#include "VisioConnectPub.h"


VisioConnectPub* VisioConnectPub::instance = nullptr; // Initialisation de l'instance unique

// Définition des variables statiques déclarées dans le .h
const int VisioConnectPub::sequenceA[4] = {0, 1, 2, 4};
const int VisioConnectPub::sequenceB[4] = {0, 1, 3, 4};
bool VisioConnectPub::sequenceCourante = true;
byte VisioConnectPub::TxByteArrCon0[FRISQUET_CONNECT_TRAME_SIZE] = {0x80, 0x7e, 0x21, 0xE0, 0x01, 0x03, 0xA0, 0x2B, 0x00, 0x04};
byte VisioConnectPub::TxByteArrCon1[FRISQUET_CONNECT_TRAME_SIZE] = {0x80, 0x7e, 0x21, 0xE0, 0x01, 0x03, 0x79, 0xE0, 0x00, 0x1C};
byte VisioConnectPub::TxByteArrCon2[FRISQUET_CONNECT_TRAME_SIZE] = {0x80, 0x7e, 0x21, 0xE0, 0x01, 0x03, 0x7A, 0x18, 0x00, 0x1C};
byte VisioConnectPub::TxByteArrCon3[FRISQUET_CONNECT_TRAME_SIZE] = {0x80, 0x7e, 0x21, 0xE0, 0x01, 0x03, 0x7A, 0x34, 0x00, 0x1C};
byte VisioConnectPub::TxByteArrCon4[FRISQUET_CONNECT_TRAME_SIZE] = {0x80, 0x7e, 0x21, 0xE0, 0x01, 0x03, 0x79, 0xFC, 0x00, 0x1C};
byte* VisioConnectPub::conMsgArrays[5] = {VisioConnectPub::TxByteArrCon0, VisioConnectPub::TxByteArrCon1, VisioConnectPub::TxByteArrCon2, VisioConnectPub::TxByteArrCon3, VisioConnectPub::TxByteArrCon4};

int VisioConnectPub::idxMessageAEnvoyer = 0;
uint8_t VisioConnectPub::numeroMessage = 0x03;

VisioConnectPub::VisioConnectPub() {
    DEBUGLN(F("VisioConnectPub - +Constructeur"));
    DEBUGLN(F("VisioConnectPub - -Constructeur"));
}

VisioConnectPub* VisioConnectPub::getInstance() {
    if (instance == nullptr) {
        instance = new VisioConnectPub();
    }
    return instance;
}

/**
 * Permet d'associer le module Frisquet Connect émulé par le Heltec
 */
bool VisioConnectPub::associerFrisquetConnect () {
    // Trame d'association pour le frisquet connect
    byte TxByteArrFriCon[10] = {0x80, 0x7e, 0x00, 0x00, 0x82, 0x41, 0x01, 0x21, 0x01, 0x02};
    // Utilise la méthode générique
    return associerModule(TxByteArrFriCon, sizeof(TxByteArrFriCon));
}

/**
 * Méthode générique d'association d'un module (ex: sonde, connect)
 * Mode opératoire : 
 *   Sur la chaudière, allez dans le menu de configuration et lancez l’association du module .
 *   Appuyez sur OK jusqu’à ce que l’écran demande d’associer le Frisquet Connect.
 *   Sur Home Assistant, allez sur l’appareil et activez l’interrupteur nommé « ass. connect ».
 *   La chaudière devrait indiquer que le Frisquet Connect est associé, et le bouton « ass. connect » doit repasser à l’état off.
 * 
 * - trameAssociation : tableau de bytes à envoyer pour l'association
 * - tailleTrame : taille du tableau
 */
bool VisioConnectPub::associerModule(byte* trameAssociation, size_t tailleTrame) {
    DEBUGLN(F("VisioConnectPub - +associerModule"));

    ModuleHeltec::affiche(F("En attente d'association..."));

    byte byteArr[RADIOLIB_SX126X_MAX_PACKET_LENGTH];
    int state = ModuleHeltec::radio.receive(byteArr, 0);
    if (state != RADIOLIB_ERR_NONE) {
        ModuleHeltec::affiche(F("Aucun message reçu ou erreur radio."));
        return false;
    }
    
    int len = ModuleHeltec::radio.getPacketLength();
    Serial.printf("RECEIVED [%2d] : ", len);
    for (int i = 0; i < len; i++)
        Serial.printf("%02X ", byteArr[i]);
    DEBUGLN();

    if (len != 11) {
        ModuleHeltec::affiche("Attendue 11; reçue : " + String(len));
        return false;
    }

    // Adapter le payload à partir du message reçu
    trameAssociation[2] = byteArr[2];
    trameAssociation[3] = byteArr[3];
    trameAssociation[4] = byteArr[4] | 0x80; // Ajouter 0x80 au 5eme byte
    trameAssociation[5] = byteArr[5];
    trameAssociation[6] = byteArr[6];

    uint8_t deviceId = byteArr[2]; // ID du device à associer

    // Sauvegarde dans la NVS et mise à jour du singleton Connect
    ModuleHeltec::saveIdConnect(deviceId);
    Connect::getInstance().setId(deviceId);

    ModuleHeltec::affiche("deviceID : " + Utils::byteArrayToHexString(&deviceId, 1));
    #ifdef DEBUG_ON
        DEBUG(F("ID du device à associer : "));
        Serial.printf("%02X ", deviceId);
        DEBUGLN();
    #endif
       DEBUG(F("Custom Network ID: "));
        for (int i = 0; i < 4; i++)
        {
          Serial.printf("%02X ", trameAssociation[len - 4 + i]);
        }
        DEBUGLN();


    // Envoi de la trame de réponse à l'association
    bool txState = envoyerTrame(trameAssociation, tailleTrame);
    if (txState != true) {
        ModuleHeltec::affiche("Erreur tx association)");
    }

    DEBUGLN(F("VisioConnectPub - -associerModule"));
    return txState;
}

/**
 * Envoi cyclique des trames vers le connect
 * Cette méthode est appelée dans la boucle principale pour envoyer les trames de manière périodique.
 */
void VisioConnectPub::loop() {

    if (Connect::getInstance().isConnectAssocie() == false) {
        // Si le connect n'est pas associé, on ne fait rien
        return;
    }
    unsigned long now = millis();

    // Réinitialisation de la séquence de trames a émettre toutes les 10 minutes
    static unsigned long millisDerniereEmissionSequence = 0;
    if (now - millisDerniereEmissionSequence >= 600000) { // 10 minutes
    //if (now - millisDerniereEmissionSequence >= 300000) { // 5 minutes
        idxMessageAEnvoyer = 0;
        sequenceCourante = !sequenceCourante;
        millisDerniereEmissionSequence = now;
    }

    // Envoi un message toutes les 2 secondes s'il y en a à envoyer
    static unsigned long millisDerniereEmissionMessage = 0;
    if (now - millisDerniereEmissionMessage >= 2000) {
        const int* sequence = sequenceCourante ? sequenceA : sequenceB;
        //S'il y a des trames à envoyer
        if (idxMessageAEnvoyer < nbMessagesSequence) {
            int trameIdx = sequence[idxMessageAEnvoyer];
            byte numeroMsg = VisioConnectPub::getNumeroMessage();
            // Met à jour les champs dynamiques de la trame
            conMsgArrays[trameIdx][2] = Connect::getInstance().getId();
            conMsgArrays[trameIdx][3] = numeroMsg;
            
            #ifdef DEBUG_ON
            Serial.printf("Envoi trame connect %d, num=%02X (seq %s)\n", trameIdx, numeroMsg, sequenceCourante ? "A" : "B");
            #endif

            
            // Si c'est la trame d'index 1, on attend la réponse 79e0
            if (trameIdx == 1) {
                extern VisioConnectSub* visioConnectSub;
                if (visioConnectSub) {
                    visioConnectSub->attendreTrame63_79e0(numeroMsg);
                }
            }
            // Si c'est la trame d'index 2, on attend la réponse 79e0
            if (trameIdx == 2) {
                extern VisioConnectSub* visioConnectSub;
                if (visioConnectSub) {
                    visioConnectSub->attendreTrame63_7a18(numeroMsg);
                }
            }

            envoyerTrame(conMsgArrays[trameIdx], FRISQUET_CONNECT_TRAME_SIZE);
            

            // Incrémente numeroMessage de 4 et gère le débordement (TODO bizarre..)
            VisioConnectPub::getNumeroMessage();
            VisioConnectPub::getNumeroMessage();
            VisioConnectPub::getNumeroMessage();
            
            idxMessageAEnvoyer++;
            millisDerniereEmissionMessage = now;
        }
    }
}

void VisioConnectPub::notifierReponse79e0() {

//TODO a voir si utile de mettre en attente la méthode loop sur la réception de la trame ou pas

}

void VisioConnectPub::notifierReponse7a18() {
//TODO a voir si utile de mettre en attente la méthode loop sur la réception de la trame ou pas
}

bool VisioConnectPub::envoyerTrame(byte* trame, size_t tailleTrame) {
    #ifdef DEBUG_ON
        if (tailleTrame > 0) {
            Serial.printf("SEND [%2d] : ", tailleTrame);
            for (int i = 0; i < tailleTrame; i++) {
                Serial.printf("%02X ", trame[i]);
            }
            Serial.println();
        }
    #endif

    // Utilise la radio pour envoyer la trame telle quelle
    int state = ModuleHeltec::radio.transmit(trame, tailleTrame);
    
    //Remet le module en mode réception
    ModuleHeltec::radio.startReceive();

    if (state == RADIOLIB_ERR_NONE) {
        return true;
    } else {
        return false;
    }
}

bool VisioConnectPub::envoyerZone(Zone* zone) {
    DEBUGLN("VisioConnectPub - +envoyerZone");
    if (!zone) return false;
    // Déterminer la valeur du mode à transmettre
    uint8_t modeValue1 = 0x00;
    bool modeValid = true;
    switch (zone->getMode()) {
        case Zone::AUTO:
            modeValue1 = 0x05;
            break;
        case Zone::CONFORT:
            modeValue1 = 0x06;
            break;
        case Zone::REDUIT:
            modeValue1 = 0x07;
            break;
        case Zone::HORS_GEL:
            modeValue1 = 0x08;
            break;
        default:
            modeValid = false;
            break;
    }
    if (!modeValid) {
        DEBUGLN("Mode zone non reconnu !");
        return false;
    }

    // Construction de la trame à envoyer
    byte trame[63] = {0x80, 0x7e, 0x00, 0x03, 0x00, 0x17, 0xA1, 0x54, 0x00, 0x18, 0xA1, 0x54, 0x00, 0x18, 0x30};
    trame[2] = Connect::getInstance().getId(); // id connect
    trame[3] = VisioConnectPub::getNumeroMessage(); // numéro de message
    trame[4] = zone->getId(); // id zone
    trame[15] = ((int) (zone->getTempConfort() * 10)) & 0xFF;
    trame[16] = ((int) (zone->getTempReduit() * 10)) & 0xFF;
    trame[17] = ((int) (zone->getTempHorsGel() * 10)) & 0xFF;
    // Mode de la zone
    // Les valeurs 0x05, 0x06, 0x07, 0x08 correspondent respectivement à AUTO, CONFORT, REDUIT, HORS_GEL
    trame[18] = modeValue1;
    trame[19] = 0x21;
    trame[20] = 0x00;
    // Ajout de la programmation journalière (7 jours, 6 octets par jour)
    // Supposons que zone->getProgrammationJournee(jour) retourne un pointeur vers un tableau de 6 octets pour chaque jour (0 = dimanche, 6 = samedi)
    for (int jour = 0; jour < 7; ++jour) {
        const uint8_t* prog = zone->getProgrammationJour(static_cast<Zone::JourSemaine>(jour));
        if (prog) {
            for (int h = 0; h < 6; ++h) {
                // On s'assure de ne pas dépasser la taille du tableau trame
                int idx = 21 + jour * 6 + h;
                if (idx < sizeof(trame)) {
                    trame[idx] = prog[h];
                }
            }
        }
    }

    bool result = envoyerTrame(trame, sizeof(trame));
    DEBUGLN("VisioConnectPub - -envoyerZone");
    return result;
}

uint8_t VisioConnectPub::getNumeroMessage() {
    uint8_t current = numeroMessage;
    numeroMessage++;
    if (numeroMessage > 0xFF) numeroMessage = 0x03;
    return current;
}

