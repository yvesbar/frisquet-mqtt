#include "ModuleHeltec.h"

// Define the static member variable
SX1262 ModuleHeltec::radio = SX1262(new Module(SS, DIO0, RST_LoRa, BUSY_LoRa));
Preferences ModuleHeltec::preferences; // Définition de la référence statique


/**
 * Initialisation du module WiFi
 */
void ModuleHeltec::initWifi(const char* ssid, const char* password) {
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    while (WiFi.waitForConnectResult() != WL_CONNECTED) {
        Serial.println("Connection Failed! Rebooting...");
        DEBUGLN(F("INIT - Erreur connexion Wifi - reboot"));
        delay(5000);
        ESP.restart();
    }

    DEBUG(F("INIT - Adresse IP : "));
    DEBUGLN(WiFi.localIP());
}

/**
 * Initialisation de l'affichage
 */
void ModuleHeltec::initAffichage() {
    DEBUGLN(F("ModuleHeltec - +initAffichage"));

    // Initialisation de l'écran OLED
    Heltec.begin(true /*DisplayEnable Enable*/, false /*LoRa Disable*/, true /*Serial Enable*/);
    Heltec.display->init();
    Heltec.display->setFont(ArialMT_Plain_10);
    Heltec.display->clear();
    Heltec.display->drawXbm(0, 0, 128, 64, myLogo);
    Heltec.display->display();

    DEBUGLN(F("ModuleHeltec - -initAffichage"));
}

/**
 * Affiche un message sur l'écran OLED
 */
void ModuleHeltec::affiche(const String& message) {
    DEBUGLN(F("ModuleHeltec - +affiche"));

    Heltec.display->clear();
    Heltec.display->setTextAlignment(TEXT_ALIGN_LEFT);
    Heltec.display->setFont(ArialMT_Plain_10);
    Heltec.display->drawString(0, 0, message);
    Heltec.display->display();

    DEBUGLN(F("ModuleHeltec - -affiche"));
}

/**
 * Initialisation du module radio
 */
void ModuleHeltec::initRadio() {
    DEBUGLN(F("ModuleHeltec - +initRadio"));

    // Configuration du module radio
    int state = radio.beginFSK();
    state = radio.setFrequency(868.96);
    state = radio.setBitRate(25.0);
    state = radio.setFrequencyDeviation(50.0);
    state = radio.setRxBandwidth(250.0);
    state = radio.setPreambleLength(4);
    state = radio.setSyncWord(network_id, sizeof(network_id));

    DEBUGLN(F("ModuleHeltec - -initRadio"));
}

/**
 * Initialisation de l'OTA
 */
void ModuleHeltec::initOTA() {
    ArduinoOTA.setHostname("ESP32Frisquet");
    ArduinoOTA.setTimeout(25); // Augmenter le délai d'attente à 25 secondes
    ArduinoOTA
        .onStart([]() {
            String type;
            if (ArduinoOTA.getCommand() == U_FLASH)
                type = "sketch";
            else // U_SPIFFS
                type = "filesystem";
            DEBUGLN("Start updating " + type);
        })
        .onEnd([]() { DEBUGLN(F("\nEnd")); })
        .onProgress([](unsigned int progress, unsigned int total) {
            Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
        })
        .onError([](ota_error_t error) {
            Serial.printf("Error[%u]: ", error);
            if (error == OTA_AUTH_ERROR) DEBUGLN(F("Auth Failed"));
            else if (error == OTA_BEGIN_ERROR) DEBUGLN(F("Begin Failed"));
            else if (error == OTA_CONNECT_ERROR) DEBUGLN(F("Connect Failed"));
            else if (error == OTA_RECEIVE_ERROR) DEBUGLN(F("Receive Failed"));
            else if (error == OTA_END_ERROR) DEBUGLN(F("End Failed"));
        });
    ArduinoOTA.begin();
}
