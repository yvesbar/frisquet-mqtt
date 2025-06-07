#ifndef VISIOCONNECTPUB_H
#define VISIOCONNECTPUB_H

#include <Arduino.h>
#include <RadioLib.h>
#include "ModuleHeltec.h"
#include "../debug.h"
#include "../Utils.h"

class VisioConnectPub {
public:
    static VisioConnectPub* getInstance(); // Méthode pour obtenir l'instance unique

    // Permet d'associer le module Frisquet Connect émulé par le Heltec
    bool associerFrisquetConnect();

    // Méthode générique d'association d'un module (ex: sonde, connect)
    bool associerModule(
        byte* trameAssociation, size_t tailleTrame
    );

private:
    VisioConnectPub(); // Constructeur privé pour le singleton
    static VisioConnectPub* instance; // Pointeur vers l'instance unique
};

#endif // VISIOCONNECTPUB_H
