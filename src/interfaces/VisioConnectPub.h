#ifndef VISIOCONNECTPUB_H
#define VISIOCONNECTPUB_H

#define FRISQUET_CONNECT_TRAME_SIZE 10

#include <Arduino.h>
#include <RadioLib.h>
#include "ModuleHeltec.h"
#include "../debug.h"
#include "../Utils.h"
#include "VisioConnectSub.h"
#include "../modele/Connect.h"

class VisioConnectPub {
public:
    static VisioConnectPub* getInstance(); // Méthode pour obtenir l'instance unique

    // Permet d'associer le module Frisquet Connect émulé par le Heltec
    bool associerFrisquetConnect();

    // Méthode générique d'association d'un module (ex: sonde, connect)
    bool associerModule(
        byte* trameAssociation, size_t tailleTrame
    );

    // Ajoute la méthode loop pour l'envoi cyclique des trames connect
    void loop();

    // Méthode appelée par VisioConnectSub quand la réponse 79e0 est reçue
    void notifierReponse79e0();
    void notifierReponse7a18();

    // Envoie une trame déjà construite
    bool envoyerTrame(byte* trame, size_t tailleTrame = 10);

    bool envoyerZone(Zone* zone);

private:
    VisioConnectPub(); // Constructeur privé pour le singleton
    static VisioConnectPub* instance; // Pointeur vers l'instance unique

    // Variables pour la gestion de l'envoi cyclique des trames vers le connect
    static const int nbMessagesSequence = 4;
    static const int sequenceA[4];
    static const int sequenceB[4];
    static bool sequenceMsg;
    static bool sequenceCourante;
    static byte TxByteArrCon0[10];
    static byte TxByteArrCon1[10];
    static byte TxByteArrCon2[10];
    static byte TxByteArrCon3[10];
    static byte TxByteArrCon4[10];
    static byte* conMsgArrays[5];

    // Variables d'état pour le loop
    static int conMsgIndex;
    static int idxMessageAEnvoyer;
    static uint8_t numeroMessage;

    uint8_t getNumeroMessage();
};

#endif // VISIOCONNECTPUB_H
