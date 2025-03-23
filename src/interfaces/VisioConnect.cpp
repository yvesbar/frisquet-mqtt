#include "VisioConnect.h"

// Initialisation de la variable statique
bool VisioConnect::receivedFlag = false;

VisioConnect::VisioConnect(Chaudiere* chaudiere) : chaudiere(chaudiere), radio(new Module(SS, DIO0, RST_LoRa, BUSY_LoRa)) {
    DEBUGLN(F("VisioConnect - +Constructeur"));
    DEBUGLN(F("VisioConnect - -Constructeur"));
}

void VisioConnect::init() {
    DEBUGLN(F("VisioConnect - +init"));
    // Initialize OLED display
    Heltec.begin(true /*DisplayEnable Enable*/, false /*LoRa Disable*/, true /*Serial Enable*/);
    Heltec.display->init();
    // Heltec.display->flipScreenVertically();
    Heltec.display->setFont(ArialMT_Plain_10);
    Heltec.display->clear();
    Heltec.display->drawXbm(0, 0, 128, 64, myLogo);
    Heltec.display->display();

    // Configuration du callback pour la réception des paquets
    radio.setPacketReceivedAction(VisioConnect::setFlag);

    // start listening for Radio packets
    int state = radio.beginFSK();
    state = radio.setFrequency(868.96);
    state = radio.setBitRate(25.0);
    state = radio.setFrequencyDeviation(50.0);
    state = radio.setRxBandwidth(250.0);
    state = radio.setPreambleLength(4);
    //TODO géréer la mémoire NVS
    state = radio.setSyncWord(network_id, sizeof(network_id));

    state = radio.startReceive();
    if (state == RADIOLIB_ERR_NONE)
    {
        DEBUGLN("Module visioConnect initialisé");
    }
    else
    {
        DEBUGLN("Module visioConnect en erreur, code : ");
        DEBUGLN(state);
    }
    DEBUGLN(F("VisioConnect - -init"));
}

void VisioConnect::setFlag() {
    DEBUGLN(F("VisioConnect - +setFlag"));
    receivedFlag = true;

    DEBUGLN(F("VisioConnect - -setFlag"));
}

void VisioConnect::lireTrame() {
    if (receivedFlag) {
        receivedFlag = false; // Réinitialise le drapeau

        // Lecture de la trame
        byte byteArr[RADIOLIB_SX126X_MAX_PACKET_LENGTH];
        int state = radio.readData(byteArr, 0);
        if (state == RADIOLIB_ERR_NONE) {
            int len = radio.getPacketLength();
            #ifdef DEBUG_ON
                Serial.printf("RECEIVED [%2d] : \n", len);
                for (int i = 0; i < len; i++) {
                    Serial.printf("%02X ", byteArr[i]);
                }
            #endif
            
            // TODO : Ajouter le traitement de la trame ici
        } 
        else if (state == RADIOLIB_ERR_CRC_MISMATCH) {
          // packet was received, but is malformed
          DEBUGLN(F("CRC error!"));
        }
        else {
          // some other error occurred
          DEBUG(F("failed, code "));
          DEBUGLN(state);
        }
    }

}

