#include "ModuleHeltec.h"

// Define the static member variable
SX1262 ModuleHeltec::radio = SX1262(new Module(SS, DIO0, RST_LoRa, BUSY_LoRa));
Preferences ModuleHeltec::preferences; // Définition de la référence statique


/**
 * Initialisation du module WiFi
 */
void ModuleHeltec::initWifi(const char* ssid, const char* password) {
    WiFi.mode(WIFI_STA);
    WiFi.setHostname("ESP32Frisquet");
    if (WIFI_IP_STATIQUE_ACTIVE) {
        IPAddress ip(WIFI_IP[0], WIFI_IP[1], WIFI_IP[2], WIFI_IP[3]);
        IPAddress gateway(WIFI_GATEWAY[0], WIFI_GATEWAY[1], WIFI_GATEWAY[2], WIFI_GATEWAY[3]);
        IPAddress subnet(WIFI_SUBNET[0], WIFI_SUBNET[1], WIFI_SUBNET[2], WIFI_SUBNET[3]);
        IPAddress dns1(WIFI_DNS1[0], WIFI_DNS1[1], WIFI_DNS1[2], WIFI_DNS1[3]);
        if (!WiFi.config(ip, gateway, subnet, dns1)) {
            DEBUGLN(F("INIT - Echec config IP statique (WiFi.config)"));
        } else {
            DEBUGLN(F("INIT - IP statique configurée"));
        }
    } else {
        DEBUGLN(F("INIT - Mode DHCP"));
    }
    WiFi.begin(ssid, password);
    while (WiFi.waitForConnectResult() != WL_CONNECTED) {
        Serial.println("Connection Failed! Rebooting...");
        DEBUGLN(F("INIT - Erreur connexion Wifi - reboot"));
        delay(5000);
        ESP.restart();
    }

    DEBUG(F("INIT - Adresse IP : "));
    DEBUGLN(WiFi.localIP());

    // Affichage de l'adresse MAC
    String mac = WiFi.macAddress();
    DEBUG(F("INIT - Adresse MAC : "));
    DEBUGLN(mac);
}

/**
 * Initialisation de l'affichage
 */
void ModuleHeltec::initAffichage() {
    DEBUGLN(F("ModuleHeltec - +initAffichage"));

    // Initialisation de l'écran OLED
    Heltec.begin(AFFICHAGE_ON /*DisplayEnable Enable*/, false /*LoRa Disable*/, true /*Serial Enable*/);
    if (AFFICHAGE_ON) { 
        Heltec.display->init();
        Heltec.display->setFont(ArialMT_Plain_10);
        Heltec.display->clear();
        Heltec.display->drawXbm(0, 0, 128, 64, myLogo);
        Heltec.display->display();
    }

    DEBUGLN(F("ModuleHeltec - -initAffichage"));
}

/**
 * Affiche un message sur l'écran OLED
 */
void ModuleHeltec::affiche(const String& message) {
    if (AFFICHAGE_ON) { 
        Heltec.display->clear();
        Heltec.display->setTextAlignment(TEXT_ALIGN_LEFT);
        Heltec.display->setFont(ArialMT_Plain_10);
        Heltec.display->drawString(0, 0, message);
        Heltec.display->display();
    }
}

/**
 * Initialisation du module radio
 */
void ModuleHeltec::initRadio() {
    DEBUGLN(F("ModuleHeltec - +initRadio"));

    // Configuration du module radio
    radio.beginFSK();
    radio.setFrequency(868.96);
    radio.setBitRate(25.0);
    radio.setFrequencyDeviation(50.0);
    radio.setRxBandwidth(250.0);
    radio.setPreambleLength(4);
    radio.setSyncWord(network_id, sizeof(network_id));

    DEBUGLN(F("ModuleHeltec - -initRadio"));
}

/**
 * Initialisation de l'OTA
 */
void ModuleHeltec::initOTA() {
    DEBUGLN(F("ModuleHeltec - +initOTA"));
    ArduinoOTA.setHostname("ESP32Frisquet");
    ArduinoOTA.setPassword("esp32frisquet"); // Définir le mot de passe pour l'OTA
    ArduinoOTA.setPort(3232); // Port par défaut pour l'OTA
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
    DEBUGLN(F("ModuleHeltec - -initOTA"));
}

/**
 * Charge les préférences NVS dans les objets concernés
 */
void ModuleHeltec::initNVS() {
    if (connect_id != 0x00) {
        saveIdConnect(connect_id);
    }
}


void ModuleHeltec::handleOTA() {
     ArduinoOTA.handle();
}

void ModuleHeltec::saveIdConnect(byte id) {
    preferences.begin("frisquet", false);
    preferences.putUChar("idConnect", id);
    preferences.end();
}

byte ModuleHeltec::loadIdConnect() {
    preferences.begin("frisquet", true);
    byte id = preferences.getUChar("idConnect", 0); // 0 par défaut si non trouvé
    preferences.end();
    return id;
}
