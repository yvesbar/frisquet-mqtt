#include "VisioConnectSub.h"
#include "../modele/ModuleH.h" // Include manquant pour ModuleH

// Initialisation de la variable statique
bool VisioConnectSub::receivedFlag = false;

VisioConnectSub::VisioConnectSub(Chaudiere* chaudiere, MqttPub* mqtt) 
    : chaudiere(chaudiere), mqttPub(mqtt) {
    DEBUGLN(F("VisioConnectSub - +Constructeur"));
    DEBUGLN(F("VisioConnectSub - -Constructeur"));
}


void VisioConnectSub::setFlag() {
    receivedFlag = true;
}


void VisioConnectSub::init() {
    DEBUGLN(F("VisioConnectSub - +init"));

   
    // Configuration du callback pour la réception des paquets
    ModuleHeltec::radio.setPacketReceivedAction(VisioConnectSub::setFlag);

    int state = ModuleHeltec::radio.startReceive();
    if (state == RADIOLIB_ERR_NONE) {
        DEBUGLN("Module visioConnect initialisé");
    } else {
        DEBUG(F("Erreur d'initialisation du module visioConnect, code : "));
        DEBUGLN(state);
    }

    DEBUGLN(F("VisioConnectSub - -init"));
}

void VisioConnectSub::lireTrame() {
    if (receivedFlag) {
        receivedFlag = false; // Réinitialise le drapeau

        // Lecture de la trame
        byte byteArr[RADIOLIB_SX126X_MAX_PACKET_LENGTH];
        int state = ModuleHeltec::radio.readData(byteArr, 0);
        if (state == RADIOLIB_ERR_NONE) {
            int len = ModuleHeltec::radio.getPacketLength();
            #ifdef DEBUG_ON
                Serial.printf("RECEIVED [%2d] : ", len);
                for (int i = 0; i < len; i++) {
                    Serial.printf("%02X ", byteArr[i]);
                }
                Serial.println();
            #endif

            switch (len) {
                case 17: lireTrame17(byteArr);
                    break;
                case 49: lireTrame49(byteArr);
                    break;
                case 23: lireTrame23(byteArr);
                    break;
                default:
                    DEBUGLN(F("Trame non reconnue"));
                    break;
            }
        } 
        else if (state == RADIOLIB_ERR_CRC_MISMATCH) {
            DEBUGLN(F("CRC error!"));
        }
        else {
            DEBUG(F("failed, code "));
            DEBUGLN(state);
        }
    }
}

float VisioConnectSub::byteToFloat(byte highByte, byte lowByte) {
    int value = (highByte << 8) | lowByte;
    return value / 10.0;
}


/**
 * Communication entre un satellite de zone et la chaudière pour transmettre : 
 * - la température ambiante
 * - la température de consigne
 * 
 * Exemple de trame : 
 *  RECEIVED [23] : 80 08 A7 14 01 17 A0 29 00 15 A0 2F 00 04 08 00 D6 00 B4 00 20 00 C6
 */
void VisioConnectSub::lireTrame23(byte* trame) {
    DEBUGLN(F("VisioConnectSub - +lireTrame23"));

    // Récupération de la zone. Si elle n'existe pas, on la crée et on pousse la conf HA pour la zone
    Zone* zone = chaudiere->getZoneById(trame[1]);
    if (zone == nullptr) {
        zone = chaudiere->addZone(trame[1]);
        if (zone == nullptr) {
            DEBUGLN(F("Zone inconnue"));
            return;
        }
        mqttPub->deployConfHAZone(zone); // Déploie la configuration HA pour la zone
    }
    
    // Extraction de la température ambiante
    float temperatureValue = byteToFloat(trame[15], trame[16]);
    if (zone->getTempAmbiance() != temperatureValue) {
        zone->setTempAmbiance(temperatureValue);
        mqttPub->publishTempAmbiante(zone, temperatureValue);
    }

    // Extraction de la température de consigne
    temperatureValue = byteToFloat(trame[17], trame[18]);
    if (zone->getTempConsigne() != temperatureValue) {
        zone->setTempConsigne(temperatureValue);
        mqttPub->publishTempConsigne(zone, temperatureValue);
    }

    DEBUGLN(F("VisioConnectSub - -lireTrame23"));
}


/**
 * Rôle de la trame ??
 * 
 * Dans le cas d'une trame émise par le module H, transmet : 
 *  - la température extérieure
 * 
 * Exemple de trame : 
 *  RECEIVED [17] : 80 25 0D 04 01 17 9C 54 00 04 A0 29 00 01 02 00 64
 *  RECEIVED [17] : 80 08 A7 08 01 17 A0 29 00 15 A0 2F 00 01 02 00 D5 
 * 
 */
void VisioConnectSub::lireTrame17(byte* trame) {
    DEBUGLN(F("VisioConnectSub - +lireTrame17"));

    if (trame[1] == ModuleH::ID) {
        // Extraction de la température extérieure
        float temperatureExterieure = byteToFloat(trame[15], trame[16]);
        if (temperatureExterieure != chaudiere->getTempExterieure()) {
            chaudiere->setTempExterieure(temperatureExterieure);
            mqttPub->publishTempExterieure(temperatureExterieure);
        }
    }

    DEBUGLN(F("VisioConnectSub - -lireTrame17"));
}


/**
 * Trame d'info de la chaudière vers les satellites de zone avec : 
 *   - la température extérieure connue de la chaudière (issue du module H ou d'un capteur extérieur)
 * 
 * Exemple de trame :
 *   RECEIVED [49] : 08 80 A7 08 81 17 2A 00 64 00 00 25 03 23 21 02 19 00 00 00 D5 00 B4 00 20 00 00 00 C6 00 C6 00 C3 00 25 00 00 00 00 04 F6 00 00 00 00 00 00 00 00
 */
void VisioConnectSub::lireTrame49(byte* trame) {
    DEBUGLN(F("VisioConnectSub - +lireTrame19"));

    if (trame[0] == Zone::ZONE1_ID || trame[0] == Zone::ZONE2_ID || trame[0] == Zone::ZONE3_ID) {
        // Extraction de la température extérieure
        float temperatureExterieure = byteToFloat(trame[7], trame[8]);
        if (temperatureExterieure != chaudiere->getTempExterieure()) {
            chaudiere->setTempExterieure(temperatureExterieure);
            mqttPub->publishTempExterieure(temperatureExterieure);
        }
    }

    DEBUGLN(F("VisioConnectSub - -lireTrame19"));
}
