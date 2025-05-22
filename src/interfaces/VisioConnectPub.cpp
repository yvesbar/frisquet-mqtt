#include "VisioConnectPub.h"

VisioConnectPub* VisioConnectPub::instance = nullptr; // Initialisation de l'instance unique

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

    //Buffer pour la réception
    byte buffer[RADIOLIB_SX126X_MAX_PACKET_LENGTH];

    DEBUGLN(F("En attente d'association...\r"));

    int state = ModuleHeltec::radio.receive(buffer, 0);
    if (state != RADIOLIB_ERR_NONE) {
        DEBUGLN(F("Aucun message reçu ou erreur radio."));
        return false;
    }
    
    int len = ModuleHeltec::radio.getPacketLength();
    #ifdef DEBUG_ON
        Serial.printf("RECEIVED [%2d] : ", len);
        for (int i = 0; i < len; i++)
            Serial.printf("%02X ", buffer[i]);
        DEBUGLN();
    #endif

    // Vérifier la longueur attendue (ici 11 : frisquet connect ou sonde externe)
    if (len != 11) {
        DEBUGLN(F("Taille du message inattendue pour l'association."));
        return false;
    }

    // Adapter le payload à partir du message reçu
    TxByteArrFriCon[2] = buffer[2];
    TxByteArrFriCon[3] = buffer[3];
    TxByteArrFriCon[4] = buffer[4] | 0x80; // Ajouter 0x80 au 5eme byte
    TxByteArrFriCon[5] = buffer[5];
    TxByteArrFriCon[6] = buffer[6];
    
    byte deviceId = buffer[2];

    // Envoi de la chaine d'association (pas de retour attendu)
    int txState = ModuleHeltec::radio.transmit(TxByteArrFriCon, sizeof(TxByteArrFriCon));

    if (txState != RADIOLIB_ERR_NONE) {
        DEBUGLN(F("Erreur lors de la transmission de la trame d'association"));
        return false;
    }

    // Les 4 dernier octets correspondent à la clef réseau
    // l'octet 3 correspond à l'ID du device
    /*for (int i = 0; i < 4; i++) {
        network_id[i] = byteArr[len - 4 + i];
    }*/


    DEBUG(F("Identifiant du périphérique "));
    #ifdef DEBUG_ON
    Serial.printf("%02X ", deviceId);
    #endif
    DEBUGLN();

    ModuleHeltec::affiche("Association OK. ID : " + Utils::byteArrayToHexString(& deviceId, 1));

    return true;
}