/*
* 1er octet = destinataire
  2ème octet = émetteur 
    0x80 : chaudière
    0x08 : Satellite zone 1
    0x09 : Satellite zone 2 (0x0a pour la zone 3 je suppose du coup)
    0x25 : Module H (vanne 4 voies + sonde extérieure filaire) qui pilote la zone 2.
*/

#ifndef __VISIOCONNECT_SUB_H_
#define __VISIOCONNECT_SUB_H_

#include <Arduino.h>
#include <heltec.h>
#include <RadioLib.h>
#include <string>
#include "../modele/Chaudiere.h"
#include "../modele/ModuleH.h"
#include "MqttPub.h"
#include "ModuleHeltec.h"


/**
 * Simule le module Visio et le module connect : interprète les trames reçues
 */
class VisioConnectSub {
    public:
        VisioConnectSub(MqttPub* mqttPub); // Constructeur avec MqttPub
        void init(); // Public method to initialize the OLED display and radio
        static void setFlag(); // Méthode statique pour indiquer qu'un paquet a été reçu
        void lireTrame(); // Méthode pour lire et traiter une trame reçue
        static bool receivedFlag; // Drapeau pour indiquer qu'un paquet a été reçu

    private:
        MqttPub* mqttPub;        // Référence à une instance de MqttPub

        void lireTrame23(byte* trame); // Méthode pour traiter une trame de 23 octets
        void lireTrame17(byte* trame); // Méthode pour traiter une trame de 17 octets
        void lireTrame49(byte* trame); // Méthode pour traiter une trame de 19 octets
        float byteToFloat(byte highByte, byte lowByte); // Conversion de bytes en float
};

#endif // __VISIOCONNECT_SUB_H_