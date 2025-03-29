#ifndef VISIOCONNECTPUB_H
#define VISIOCONNECTPUB_H

#include <Arduino.h>
#include <RadioLib.h>
#include "ModuleHeltec.h"
#include "../debug.h"
#include "../Utils.h"

class VisioConnectPub {
public:
    VisioConnectPub();

    // Permet d'associer le module Frisquet Connect émulé par le Heltec
    bool associerFrisquetConnect();

private:

};

#endif // VISIOCONNECTPUB_H
