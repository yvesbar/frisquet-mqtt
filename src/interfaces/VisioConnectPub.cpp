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
    #ifdef DEBUG_ON
        DEBUG(F("ID du device à associer : "));
        Serial.printf("%02X ", deviceId);
        DEBUGLN();
    #endif


    // Envoi de la trame de réponse à l'association
    int txState = ModuleHeltec::radio.transmit(trameAssociation, tailleTrame);
    if (txState != RADIOLIB_ERR_NONE) {
        ModuleHeltec::affiche("Erreur tx association)");
        return false;
    }

    DEBUGLN(F("VisioConnectPub - -associerModule"));
    return true;
}

