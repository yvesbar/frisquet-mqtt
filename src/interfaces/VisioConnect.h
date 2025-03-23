#ifndef __VISIOCONNECT_H_
#define __VISIOCONNECT_H_

#include <Arduino.h>
#include <heltec.h>
#include <RadioLib.h>
#include <string>
#include "image.h"
#include "../modele/Chaudiere.h"

class VisioConnect {
    public:
        VisioConnect(Chaudiere* chaudiere);
        void init(); // Public method to initialize the OLED display and radio
        static void setFlag(); // Méthode statique pour indiquer qu'un paquet a été reçu
        void lireTrame(); // Méthode pour lire et traiter une trame reçue
        static bool receivedFlag; // Drapeau pour indiquer qu'un paquet a été reçu

    private:
        Chaudiere* chaudiere; // Référence à une instance de Chaudiere
        SX1262 radio;         // Module radio en tant que membre privé

};

#endif // __VISIOCONNECT_H_