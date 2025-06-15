#include "VisioConnectSub.h"
#include "../modele/ModuleH.h" // Include manquant pour ModuleH
#include "VisioConnectPub.h"

// Initialisation de la variable statique
bool VisioConnectSub::receivedFlag = false;

VisioConnectSub::VisioConnectSub(MqttPub* mqtt) 
    : mqttPub(mqtt) {
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
                case 63: lireTrame63(byteArr);
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

float VisioConnectSub::bytesToTemperature(byte highByte, byte lowByte) {
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
    Zone* zone = getZone(trame[1]);
   
    // Extraction de la température ambiante
    float temperatureValue = bytesToTemperature(trame[15], trame[16]);
    if (zone->getTempAmbiance() != temperatureValue) {
        zone->setTempAmbiance(temperatureValue);
        mqttPub->publishTempAmbiante(zone, temperatureValue);
    }

    // Extraction de la température de consigne
    temperatureValue = bytesToTemperature(trame[17], trame[18]);
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
        float temperatureExterieure = bytesToTemperature(trame[15], trame[16]);
        if (temperatureExterieure != Chaudiere::getInstance().getTempExterieure()) {
            Chaudiere::getInstance().setTempExterieure(temperatureExterieure);
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
        float temperatureExterieure = bytesToTemperature(trame[7], trame[8]);
        if (temperatureExterieure != Chaudiere::getInstance().getTempExterieure()) {
            Chaudiere::getInstance().setTempExterieure(temperatureExterieure);
            mqttPub->publishTempExterieure(temperatureExterieure);
        }
    }

    DEBUGLN(F("VisioConnectSub - -lireTrame19"));
}

/** Indique qu'on attend une réponse à la trame 79e0 avec un numéroDeMessage bien précis */
void VisioConnectSub::attendreTrame63_79e0(byte numeroMessage) {
    idAttendu79e0 = numeroMessage;
}

void VisioConnectSub::attendreTrame63_7a18(byte numeroMessage) {
    idAttendu7a18 = numeroMessage;
}

/**
 * Trame du visioConnect : trame d'infos
 */
void VisioConnectSub::lireTrame63(byte* trame) {
    // On ne traite que les trames 79e0 (identifiants spécifiques)
    if (trame[0] == 0x7e && trame[1] == 0x80 && trame[3] == idAttendu79e0 && trame[4] == 0x81 && trame[5] == 0x03) {
        idAttendu79e0 = 0;
        decodeTrame63_infosCapteurs(trame);
        VisioConnectPub::getInstance()->notifierReponse79e0();
    }
    // Ajout du décodage de la trame 7a18
    else if (trame[0] == 0x7e && trame[1] == 0x80 && trame[3] == idAttendu7a18 && trame[4] == 0x81 && trame[5] == 0x03) {
        idAttendu7a18 = 0;
        decodeTrame63_infosConso(trame);
        VisioConnectPub::getInstance()->notifierReponse7a18();
    }
}


/**
 * Trame 63 : infos capteurs
 * 
 * RECEIVED [63] : 7E 80 4B 49 81 03 38 02 8D 01 4F 01 15 04 F6 04 F6 01 5E 00 00 00 00 00 00 00 00 20 00 00 11 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 0D 00 65 94
 * RECEIVED [63] : 7E 80 4B 50 81 03 38 02 88 01 3A 01 09 04 F6 04 F6 01 45 00 00 00 00 00 00 00 00 20 00 00 11 00 00 00 00 00 00 00 00 00 00 00 00 00 C4 04 F6 04 F6 01 00 00 C6 00 C6 00 C3 00 00 00 00 05 0A
 */
void VisioConnectSub::decodeTrame63_infosCapteurs(byte* trame) {
    int decimalValue;
    Zone* zone;

    // Extraction de la température de l'ECS (Eau Chaude Sanitaire)
    if (Chaudiere::getInstance().isEcsActive()) {
        decimalValue = bytesToTemperature(trame[7], trame[8]);
        if (decimalValue != Chaudiere::getInstance().getTempECS()) {
            Chaudiere::getInstance().setTempECS(decimalValue);
            this->mqttPub->publishTempECS(decimalValue);
        }
    }

    // Extraction de la température du corps de chauffe
    decimalValue = bytesToTemperature(trame[9], trame[10]);
    if (decimalValue != Chaudiere::getInstance().getTempCorpsDeChauffe()) {
        Chaudiere::getInstance().setTempCorpsDeChauffe(decimalValue);
        this->mqttPub->publishTempCorpsDeChauffe(decimalValue);
    }

    // Zone 1
    zone = getZone(Zone::ZONE1_ID);
    decimalValue = bytesToTemperature(trame[11], trame[12]);
    if (zone->getTempDepart() != decimalValue) {
        zone->setTempDepart(decimalValue);
        this->mqttPub->publishTempDepart(decimalValue);
    }
    decimalValue = bytesToTemperature(trame[43], trame[44]);
    if (zone->getTempAmbiance() != decimalValue) {
        zone->setTempAmbiance(decimalValue);
        this->mqttPub->publishTempAmbiante(zone, decimalValue);
    }
    decimalValue = bytesToTemperature(trame[55], trame[56]);
    if (zone->getTempConsigne() != decimalValue) {
        zone->setTempConsigne(decimalValue);
        this->mqttPub->publishTempConsigne(zone, decimalValue);
    }

    // Zone 2
    if (bytesToTemperature(trame[45], trame[46]) != 127.0) {
        zone = getZone(Zone::ZONE2_ID);
        decimalValue = bytesToTemperature(trame[13], trame[14]);
        if (zone->getTempDepart() != decimalValue) {
            zone->setTempDepart(decimalValue);
            this->mqttPub->publishTempDepart(decimalValue);
        }
        decimalValue = bytesToTemperature(trame[45], trame[46]);
        if (zone->getTempAmbiance() != decimalValue) {
            zone->setTempAmbiance(decimalValue);
            this->mqttPub->publishTempAmbiante(zone, decimalValue);
        }
        decimalValue = bytesToTemperature(trame[57], trame[58]);
        if (zone->getTempConsigne() != decimalValue) {
            zone->setTempConsigne(decimalValue);
            this->mqttPub->publishTempConsigne(zone, decimalValue);
        }
    }

    // Zone 3
    if (bytesToTemperature(trame[47], trame[48]) != 127.0) {
        zone = getZone(Zone::ZONE2_ID);
        decimalValue = bytesToTemperature(trame[15], trame[16]);
        if (zone->getTempDepart() != decimalValue) {
            zone->setTempDepart(decimalValue);
            this->mqttPub->publishTempDepart(decimalValue);
        }
        decimalValue = bytesToTemperature(trame[47], trame[48]);
        if (zone->getTempAmbiance() != decimalValue) {
            zone->setTempAmbiance(decimalValue);
            this->mqttPub->publishTempAmbiante(zone, decimalValue);
        }
        decimalValue = bytesToTemperature(trame[59], trame[60]);
        if (zone->getTempConsigne() != decimalValue) {
            zone->setTempConsigne(decimalValue);
            this->mqttPub->publishTempConsigne(zone, decimalValue);
        }
    }

    // Extraction de la température extérieure
    float temperatureExterieure = bytesToTemperature(trame[61], trame[62]);
    if (temperatureExterieure != Chaudiere::getInstance().getTempExterieure()) {
        Chaudiere::getInstance().setTempExterieure(temperatureExterieure);
        mqttPub->publishTempExterieure(temperatureExterieure);
    }
}

/**
 * Récupère une zone ou la créé la première fois qu'une trame la concernant est décodée
 */
Zone* VisioConnectSub::getZone(byte idZone) {
    Zone* zone = Chaudiere::getInstance().getZoneById(idZone);
    if (zone == nullptr) {
        zone = Chaudiere::getInstance().addZone(idZone);
        if (zone == nullptr) {
            DEBUGLN(F("Zone inconnue"));
            return nullptr;
        }
        this->mqttPub->deployConfHAZone(zone);
    }
    return zone;
}

void VisioConnectSub::decodeTrame63_infosConso(byte* trame) {
    int decimalValue1 = trame[27] << 8 | trame[28];
    this->mqttPub->publishConsoGazCh(decimalValue1);
    if (Chaudiere::getInstance().isEcsActive()) {
        int decimalValue2 = trame[25] << 8 | trame[26];
        this->mqttPub->publishConsoGazECS(decimalValue2);
    }
}
