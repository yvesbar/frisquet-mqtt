#include <Arduino.h>
#include <RadioLib.h>
#include <PubSubClient.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <heltec.h>
#include <Preferences.h>
#include "image.h"
#include "conf.h"

SX1262 radio = new Module(SS, DIO0, RST_LoRa, BUSY_LoRa);
Preferences preferences;
unsigned long lastTxExtSonTime = 0;            // Variable dernière transmission sonde
const unsigned long txExtSonInterval = 600000; // Interval de transmission en millisecondes (10 minutes)
String DateTimeRes;
String tempAmbiante;
String tempExterieure;
String tempConsigne;
String modeFrisquet;
String assSonFrisquet;
String assConFrisquet;
String eraseNvsFrisquet;
String byteArrayToHexString(uint8_t *byteArray, int length);
byte extSonTempBytes[2];
byte sonMsgNum = 0x01;
int counter = 0;
uint8_t custom_network_id[4];
uint8_t custom_extSon_id;
uint8_t custom_friCon_id;
float temperatureValue;
float temperatureconsValue;
float extSonVal;
WiFiClient espClient;
PubSubClient client(espClient);

// Drapeaux pour indiquer si les données ont changé
bool tempAmbianteChanged = false;
bool tempExterieureChanged = false;
bool tempConsigneChanged = false;
bool modeFrisquetChanged = false;
bool assSonFrisquetChanged = false;
bool assConFrisquetChanged = false;
// Constantes pour les topics MQTT
const char *TEMP_AMBIANTE1_TOPIC = "homeassistant/sensor/frisquet/tempAmbiante1/state";
const char *TEMP_EXTERIEURE_TOPIC = "homeassistant/sensor/frisquet/tempExterieure/state";
const char *TEMP_CONSIGNE1_TOPIC = "homeassistant/sensor/frisquet/tempConsigne1/state";
const char *MODE_TOPIC = "homeassistant/select/frisquet/mode/set";
const char *ASS_SON_TOPIC = "homeassistant/switch/frisquet/asssonde/set";
const char *ASS_CON_TOPIC = "homeassistant/switch/frisquet/assconnect/set";
const char *RES_NVS_TOPIC = "homeassistant/switch/frisquet/erasenvs/set";
uint8_t TempExTx[] = {0x80, 0x20, 0x00, 0x00, 0x01, 0x17, 0x9c, 0x54, 0x00, 0x04, 0xa0, 0x29, 0x00, 0x01, 0x02, 0x00, 0x00}; // envoi température
byte TxByteArr[10] = {0x80, 0x20, 0x00, 0x00, 0x82, 0x41, 0x01, 0x21, 0x01, 0x02};                                           // association Sonde exterieure
byte TxByteArrFriCon[10] = {0x80, 0x7e, 0x00, 0x00, 0x82, 0x41, 0x01, 0x21, 0x01, 0x02};                                     // association Sonde exterieure

//****************************************************************************
// Fonction pour publier un message MQTT
void publishMessage(const char *topic, const char *payload)
{
  if (!client.publish(topic, payload))
  {
    Serial.println(F("Failed to publish message to MQTT"));
  }
}
//****************************************************************************
void eraseNvs()
{
  preferences.begin("net-conf", false); // Ouvrir la mémoire NVS en mode lecture/écriture
  preferences.clear();                  // Effacer complètement la mémoire NVS
  publishMessage("homeassistant/switch/frisquet/erasenvs/state", "OFF");
  eraseNvsFrisquet = "OFF";
}
//****************************************************************************
void initNvs()
{
  // Démarre l'instance de NVS
  preferences.begin("net-conf", false); // "customId" est le nom de l'espace de stockage
  // Déclare l'id de sonde externe et du connect
  custom_extSon_id = (extSon_id == 0xFF) ? preferences.getUChar("son_id", 0) : extSon_id;
  custom_friCon_id = (friCon_id == 0xFF) ? preferences.getUChar("con_id", 0) : friCon_id;
  // Vérifie si la clé "custom_network_id" existe dans NVS
  size_t custom_network_id_size = preferences.getBytes("net_id", custom_network_id, sizeof(custom_network_id));
  // Vérifie si custom_network_id est différent de {0xFF, 0xFF, 0xFF, 0xFF} ou si la clé n'existe pas dans NVS
  if (custom_network_id_size != sizeof(custom_network_id) || memcmp(custom_network_id, "\xFF\xFF\xFF\xFF", sizeof(custom_network_id)) == 0)
  {
    // Copie la valeur de network_id ou def_Network_id dans custom_network_id
    memcpy(custom_network_id, (memcmp(network_id, "\xFF\xFF\xFF\xFF", sizeof(network_id)) != 0) ? network_id : def_Network_id, sizeof(custom_network_id));
    // Enregistre custom_network_id dans NVS
    preferences.putBytes("net_id", custom_network_id, sizeof(custom_network_id));
    preferences.end(); // Ferme la mémoire NVS
  }
}
//****************************************************************************
void sendTxByteArr()
{
  int state = radio.transmit(TxByteArr, sizeof(TxByteArr));
  if (state == RADIOLIB_ERR_NONE)
  {
    Serial.println(F("Transmission réussie"));
    for (int i = 0; i < sizeof(TxByteArr); i++)
    {
      Serial.printf("%02X ", TxByteArr[i]); // Serial.print(TempExTx[i], HEX);
      Serial.print(" ");
    }
    Serial.println();
  }
  else
  {
    Serial.println(F("Erreur lors de la transmission"));
  }
}
//****************************************************************************
void updateDisplay()
{
  if (tempAmbianteChanged || tempExterieureChanged || tempConsigneChanged || modeFrisquetChanged)
  {
    Heltec.display->clear();
    Heltec.display->drawString(0, 0, "Net: " + byteArrayToHexString(custom_network_id, sizeof(custom_network_id)) + " Son: " + byteArrayToHexString(&custom_extSon_id, 1) + " Con: " + byteArrayToHexString(&custom_friCon_id, 1));
    Heltec.display->drawString(0, 11, "Temp Ambiante: " + tempAmbiante + "°C");
    Heltec.display->drawString(0, 22, "Temp Exterieure: " + tempExterieure + "°C");
    Heltec.display->drawString(0, 33, "Temp Consigne: " + tempConsigne + "°C");
    Heltec.display->drawString(0, 44, "Mode: " + modeFrisquet);

    Heltec.display->display();
    tempAmbianteChanged = false;
    tempExterieureChanged = false;
    tempConsigneChanged = false;
    modeFrisquetChanged = false;
  }
  else if (assSonFrisquetChanged || assConFrisquetChanged)
  {
    Heltec.display->clear();
    Heltec.display->drawString(0, 0, "Net: " + byteArrayToHexString(custom_network_id, sizeof(custom_network_id)) + " Son: " + byteArrayToHexString(&custom_extSon_id, 1) + " Con: " + byteArrayToHexString(&custom_friCon_id, 1));
    Heltec.display->drawString(0, 11, "Ass. sonde en cours: " + assSonFrisquet);
    Heltec.display->drawString(0, 22, "Ass. connect en cours: " + assConFrisquet);

    Heltec.display->display();
    assSonFrisquetChanged = false;
    assConFrisquetChanged = false;
  }
}
//****************************************************************************
void callback(char *topic, byte *payload, unsigned int length)
{
  // Convertir le payload en une chaîne de caractères
  char message[length + 1];
  strncpy(message, (char *)payload, length);
  message[length] = '\0';

  // Vérifier le topic et mettre à jour les variables globales
  if (strcmp(topic, TEMP_AMBIANTE1_TOPIC) == 0)
  {
    if (tempAmbiante != String(message))
    {
      tempAmbiante = String(message);
      tempAmbianteChanged = true;
    }
  }
  else if (strcmp(topic, TEMP_EXTERIEURE_TOPIC) == 0)
  {
    if (tempExterieure != String(message))
    {
      tempExterieure = String(message);
      extSonVal = tempExterieure.toFloat() * 10;
      int extSonTemp = int(extSonVal);
      extSonTempBytes[0] = (extSonTemp >> 8) & 0xFF; // Octet de poids fort
      extSonTempBytes[1] = extSonTemp & 0xFF;        // Octet de poids faible
      tempExterieureChanged = true;
    }
  }
  else if (strcmp(topic, TEMP_CONSIGNE1_TOPIC) == 0)
  {
    if (tempConsigne != String(message))
    {
      tempConsigne = String(message);
      tempConsigneChanged = true;
    }
  }
  else if (strcmp(topic, MODE_TOPIC) == 0)
  {
    if (modeFrisquet != String(message))
    {
      modeFrisquet = String(message);
      modeFrisquetChanged = true;
      client.publish("homeassistant/select/frisquet/mode/state", message);
    }
  }
  else if (strcmp(topic, ASS_SON_TOPIC) == 0)
  {
    if (assSonFrisquet != String(message))
    {
      assSonFrisquet = String(message);
      assSonFrisquetChanged = true;
      client.publish("homeassistant/switch/frisquet/asssonde/state", message);
    }
  }
  else if (strcmp(topic, ASS_CON_TOPIC) == 0)
  {
    if (assConFrisquet != String(message))
    {
      assConFrisquet = String(message);
      assConFrisquetChanged = true;
      client.publish("homeassistant/switch/frisquet/assconnect/state", message);
    }
  }
  else if (strcmp(topic, RES_NVS_TOPIC) == 0)
  {
    if (eraseNvsFrisquet != String(message))
    {
      eraseNvsFrisquet = String(message);
      assConFrisquetChanged = true;
      client.publish("homeassistant/switch/frisquet/erasenvs/state", message);
    }
  }
}
//****************************************************************************
void txConfiguration()
{
  int state = radio.beginFSK();
  state = radio.setFrequency(868.96);
  state = radio.setBitRate(25.0);
  state = radio.setFrequencyDeviation(50.0);
  state = radio.setRxBandwidth(250.0);
  state = radio.setPreambleLength(4);
  state = radio.setSyncWord(custom_network_id, sizeof(custom_network_id));
}
//****************************************************************************
void connectToMqtt()
{
  while (!client.connected())
  {
    Serial.println(F("Connecting to MQTT..."));
    if (client.connect("ESP32 Frisquet", mqttUsername, mqttPassword))
    {
      Serial.println(F("Connected to MQTT"));
    }
    else
    {
      Serial.print(F("Failed to connect to MQTT, rc="));
      Serial.print(F(client.state()));
      Serial.println(F(" Retrying in 5 seconds..."));
      delay(5000);
    }
  }
}
//****************************************************************************
void connectToSensor(const char *topic, const char *name)
{
  char configTopic[60];
  char configPayload[350];
  snprintf(configTopic, sizeof(configTopic), "homeassistant/sensor/frisquet/%s/config", topic);
  snprintf(configPayload, sizeof(configPayload), R"(
{
  "uniq_id": "frisquet_%s",
  "name": "Frisquet - Temperature %s",
  "state_topic": "homeassistant/sensor/frisquet/%s/state",
  "unit_of_measurement": "°C",
  "device_class": "temperature",
  "device":{"ids":["FrisquetConnect"],"mf":"Frisquet","name":"Frisquet Connect","mdl":"Frisquet Connect"}
}
)",
           topic, name, topic);
  client.publish(configTopic, configPayload);
}
//****************************************************************************
void connectToSwitch(const char *topic, const char *name)
{
  char configTopic[60];
  char configPayload[400];
  snprintf(configTopic, sizeof(configTopic), "homeassistant/switch/frisquet/%s/config", topic);
  snprintf(configPayload, sizeof(configPayload), R"(
{
  "uniq_id": "frisquet_%s",
  "name": "Frisquet - %s",
  "state_topic": "homeassistant/switch/frisquet/%s/state",
  "command_topic": "homeassistant/switch/frisquet/%s/set",
  "payload_on": "ON",
  "payload_off": "OFF",
  "device":{"ids":["FrisquetConnect"],"mf":"Frisquet","name":"Frisquet Connect","mdl":"Frisquet Connect"}
}
)",
           topic, name, topic, topic);
  client.publish(configTopic, configPayload);
}
//****************************************************************************
void connectToTopic()
{
  connectToSensor("tempAmbiante1", "ambiante Z1");
  connectToSensor("tempExterieure", "exterieure");
  connectToSensor("tempConsigne1", "consigne Z1");
  connectToSwitch("asssonde", "ass. sonde");
  connectToSwitch("assconnect", "ass. connect");
  connectToSwitch("erasenvs", "erase NVS");

  if (sensorZ2 == true)
  {
    connectToSensor("tempAmbiante2", "ambiante Z2");
    connectToSensor("tempConsigne2", "consigne Z2");
    connectToSensor("tempCDC", "CDC");
    connectToSensor("tempECS", "ECS");
    connectToSensor("tempDepart", "départ");
  }
  // Configuration récupération Payload
  char payloadConfigTopic[] = "homeassistant/sensor/frisquet/payload/config";
  char payloadConfigPayload[] = R"(
{
  "name": "Frisquet - Payload",
  "state_topic": "homeassistant/sensor/frisquet/payload/state",
  "device":{"ids":["FrisquetConnect"],"mf":"Frisquet","name":"Frisquet Connect","mdl":"Frisquet Connect"}
}
)";
  client.publish(payloadConfigTopic, payloadConfigPayload);

  // Publier le message de configuration pour MQTT du mode
  char modeConfigTopic[] = "homeassistant/select/frisquet/mode/config";
  char modeConfigPayload[] = R"({
        "uniq_id": "frisquet_mode",
        "name": "Frisquet - Mode",
        "state_topic": "homeassistant/select/frisquet/mode/state",
        "command_topic": "homeassistant/select/frisquet/mode/set",
        "options": ["Auto", "Confort", "Réduit", "Hors gel"],
        "device":{"ids":["FrisquetConnect"],"mf":"Frisquet","name":"Frisquet Connect","mdl":"Frisquet Connect"}
      })";
  client.publish(modeConfigTopic, modeConfigPayload, true); // true pour retenir le message
  // Souscrire aux topics temp ambiante, consigne, ext et mode
  client.subscribe(MODE_TOPIC);
  client.subscribe(ASS_SON_TOPIC);
  client.subscribe(ASS_CON_TOPIC);
  client.subscribe(TEMP_AMBIANTE1_TOPIC);
  client.subscribe(TEMP_EXTERIEURE_TOPIC);
  client.subscribe(TEMP_CONSIGNE1_TOPIC);
}
void setDefaultNetwork()
{
  memcpy(custom_network_id, def_Network_id, sizeof(def_Network_id));
}
//****************************************************************************
void txExtSonTemp()
{
  sonMsgNum += 4;
  // Remplacer les bytes spécifiés dans TempExTx
  TempExTx[2] = custom_extSon_id;
  TempExTx[3] = sonMsgNum;
  TempExTx[15] = extSonTempBytes[0]; // Remplacer le 16ème byte par l'octet de poids fort de extSonTemp
  TempExTx[16] = extSonTempBytes[1]; // Remplacer le 17ème byte par l'octet de poids faible de extSonTemp
  // Afficher le payload dans la console
  Serial.print("Payload Sonde transmit: ");
  for (int i = 0; i < sizeof(TempExTx); i++)
  {
    Serial.printf("%02X ", TempExTx[i]);
    Serial.print(" ");
  }
  Serial.println();
  // Transmettre la chaine TempExTx
  int state = radio.transmit(TempExTx, sizeof(TempExTx));
}
//****************************************************************************
bool associateDevice(
    uint8_t *deviceTxArr, size_t deviceTxArrLen, // Tableau et taille de la payload d'association
    const uint8_t *defaultNetworkId,             // Réseau par défaut
    const char *idKey,                           // Clé NVS pour stocker l'ID du device (ex: "son_id" ou "con_id")
    const char *assCommandTopic,                 // Topic MQTT sur lequel on a reçu la demande d'association (ex: ASS_SON_TOPIC)
    const char *assStateTopic                    // Topic MQTT pour publier l'état OFF après association
)
{
  // Vérifier si le réseau est par défaut, sinon le réinitialiser
  if (memcmp(custom_network_id, defaultNetworkId, sizeof(custom_network_id)) != 0)
  {
    setDefaultNetwork();
    txConfiguration();
  }

  Serial.print(F("En attente d'association...\r"));

  byte byteArr[RADIOLIB_SX126X_MAX_PACKET_LENGTH];
  int state = radio.receive(byteArr, 0);
  if (state == RADIOLIB_ERR_NONE)
  {
    int len = radio.getPacketLength();
    Serial.printf("RECEIVED [%2d] : ", len);
    for (int i = 0; i < len; i++)
      Serial.printf("%02X ", byteArr[i]);
    Serial.println();

    // Vérifier la longueur attendue (ici 11 comme pour l'extSon) ou adapter selon le device
    if (len == 11)
    {
      // Adapter le payload à partir du message reçu
      deviceTxArr[2] = byteArr[2];
      deviceTxArr[3] = byteArr[3];
      deviceTxArr[4] = byteArr[4] | 0x80; // Ajouter 0x80 au 5eme byte

      delay(100);
      // Envoi de la chaine d'association
      int txState = radio.transmit(deviceTxArr, deviceTxArrLen);
      if (txState == RADIOLIB_ERR_NONE)
      {
        // Mise à jour du custom_network_id
        for (int i = 0; i < 4; i++)
        {
          custom_network_id[i] = byteArr[len - 4 + i];
        }

        preferences.begin("net-conf", false);
        preferences.putBytes("net_id", custom_network_id, sizeof(custom_network_id));
        preferences.putUChar(idKey, byteArr[2]); // Stocker l'ID du device
        uint8_t deviceId = preferences.getUChar(idKey, 0);
        preferences.end();

        Serial.print(F("Custom Network ID: "));
        for (int i = 0; i < sizeof(custom_network_id); i++)
        {
          Serial.printf("%02X ", custom_network_id[i]);
        }
        Serial.println();

        Serial.print(F("Custom Device ID: "));
        Serial.printf("%02X ", deviceId);
        Serial.println();

        // Publier sur MQTT pour indiquer que l'association est terminée (mettre l'état OFF)
        publishMessage(assCommandTopic, "OFF");
        publishMessage(assStateTopic, "OFF");

        Serial.println(F("Association effectuée !"));
        txConfiguration();
        Serial.println(F("Reprise de la boucle initiale"));
        return true;
      }
      else
      {
        Serial.println(F("Erreur lors de la transmission de la trame d'association"));
        return false;
      }
    }
    else
    {
      Serial.println(F("Taille du message inattendue pour l'association."));
      return false;
    }
  }
  else
  {
    Serial.println(F("Aucun message reçu ou erreur radio."));
    return false;
  }
}
//****************************************************************************
void assExtSon()
{

  associateDevice(
      TxByteArr, sizeof(TxByteArr),
      def_Network_id,
      "son_id",
      ASS_SON_TOPIC,
      "homeassistant/switch/frisquet/asssonde/state");

  // Mettre à jour de la variable sonde
  assSonFrisquet = "OFF";
}
//****************************************************************************
void assFriCon()
{

  associateDevice(
      TxByteArrFriCon, sizeof(TxByteArrFriCon),
      def_Network_id,
      "con_id",
      ASS_CON_TOPIC,
      "homeassistant/switch/frisquet/assconnect/state");

  // Mettre à jour la variable globale correspondante si vous en avez une.
  assConFrisquet = "OFF";
}
//****************************************************************************
void initOTA();
//****************************************************************************
void setup()
{
  Serial.begin(115200);
  Serial.println(F("Booting"));
  WiFi.mode(WIFI_STA);
  WiFi.setHostname("ESP32Frisquet");
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED)
  {
    Serial.println(F("Connection Failed! Rebooting..."));
    delay(5000);
    ESP.restart();
  }

  initNvs(); // écrit dans la nvs les bytes nécéssaires

  // Initialize OLED display
  Heltec.begin(true /*DisplayEnable Enable*/, false /*LoRa Disable*/, true /*Serial Enable*/);
  Heltec.display->init();
  Heltec.display->flipScreenVertically();
  Heltec.display->setFont(ArialMT_Plain_10);
  Heltec.display->clear();
  Heltec.display->drawXbm(0, 0, 128, 64, myLogo);
  Heltec.display->display();

  txConfiguration();
  initOTA();
  Serial.println(F("Ready"));
  Serial.print(F("IP address: "));
  Serial.println(WiFi.localIP());
  // Initialisation de la connexion MQTT
  client.setServer(mqttServer, mqttPort);
  client.setBufferSize(2048);
  connectToMqtt();
  connectToTopic();
  client.setCallback(callback);
  preferences.end(); // Fermez la mémoire NVS ici
}
//****************************************************************************
void handleRadioPacket(byte *byteArr, int len)
{
  Serial.printf("RECEIVED [%2d] : ", len);
  char message[255];
  message[0] = '\0';

  if (len == 23)
  { // Check if the length is 23 bytes

    // Extract bytes 16 and 17
    int decimalValueTemp = byteArr[15] << 8 | byteArr[16];
    float temperatureValue = decimalValueTemp / 10.0;
    // Extract bytes 18 and 19
    int decimalValueCons = byteArr[17] << 8 | byteArr[18];
    float temperatureconsValue = decimalValueCons / 10.0;
    // Publish temperature to the "frisquet_temperature" MQTT topic
    char temperaturePayload[10];
    snprintf(temperaturePayload, sizeof(temperaturePayload), "%.2f", temperatureValue);
    publishMessage(TEMP_AMBIANTE1_TOPIC, temperaturePayload);
    // Publish temperature to the "tempconsigne" MQTT topic
    char tempconsignePayload[10];
    snprintf(tempconsignePayload, sizeof(tempconsignePayload), "%.2f", temperatureconsValue);
    publishMessage(TEMP_CONSIGNE1_TOPIC, tempconsignePayload);
  }
  for (int i = 0; i < len; i++)
  {
    sprintf(message + strlen(message), "%02X ", byteArr[i]);
    Serial.printf("%02X ", byteArr[i]);
  }
  if (!client.publish("homeassistant/sensor/frisquet/payload/state", message))
  {
    Serial.println(F("Failed to publish Payload to MQTT"));
  }
  Serial.println(F(""));
}
//****************************************************************************
void loop()
{
  // DateTime();
  if (eraseNvsFrisquet == "ON")
  {
    eraseNvs();
    initNvs();
  }
  else if (assSonFrisquet == "ON")
  {
    assExtSon();
  }
  else if (assConFrisquet == "ON")
  {
    assFriCon();
  }
  else
  {
    unsigned long currentTime = millis();
    // Vérifier si 10 minutes se sont écoulées depuis la dernière transmission
    if (currentTime - lastTxExtSonTime >= txExtSonInterval)
    {
      // Vérifier si custom_extSon_id n'est pas égal à 0 byte
      if (memcmp(custom_network_id, "\xFF\xFF\xFF\xFF", 4) != 0 && custom_extSon_id != 0x00)
      {
        txExtSonTemp(); // Appeler la fonction pour transmettre les données
      }
      else
      {
        Serial.println(F("Id sonde externe non connue"));
      }
      // Mettre à jour le temps de la dernière transmission
      lastTxExtSonTime = currentTime;
    }
    if (!client.connected())
    {
      connectToMqtt();
    }
    ArduinoOTA.handle();

    // Compteur pour limiter la déclaration des topic
    if (counter >= 100)
    {
      connectToTopic();
      counter = 0;
    }
    counter++;

    byte byteArr[RADIOLIB_SX126X_MAX_PACKET_LENGTH];
    int state = radio.receive(byteArr, 0);
    if (state == RADIOLIB_ERR_NONE)
    {
      int len = radio.getPacketLength();
      handleRadioPacket(byteArr, len);
    }
  }
  client.loop();
  updateDisplay(); // Mettre à jour l'affichage si nécessaire
}
//************************************************************
String byteArrayToHexString(uint8_t *byteArray, int length)
{
  String result = "";
  for (int i = 0; i < length; i++)
  {
    char hex[3];
    sprintf(hex, "%02X", byteArray[i]);
    result += hex;
  }
  return result;
}
//************************************************************
void initOTA()
{
  ArduinoOTA.setHostname("ESP32Frisquet");
  ArduinoOTA.setTimeout(25); // Augmenter le délai d'attente à 25 secondes
  ArduinoOTA
      .onStart([]()
               {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH)
      type = "sketch";
    else // U_SPIFFS
      type = "filesystem";
    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    Serial.println("Start updating " + type); })
      .onEnd([]()
             { Serial.println(F("\nEnd")); })
      .onProgress([](unsigned int progress, unsigned int total)
                  { Serial.printf("Progress: %u%%\r", (progress / (total / 100))); })
      .onError([](ota_error_t error)
               {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println(F("Auth Failed"));
    else if (error == OTA_BEGIN_ERROR) Serial.println(F("Begin Failed"));
    else if (error == OTA_CONNECT_ERROR) Serial.println(F("Connect Failed"));
    else if (error == OTA_RECEIVE_ERROR) Serial.println(F("Receive Failed"));
    else if (error == OTA_END_ERROR) Serial.println(F("End Failed")); });
  ArduinoOTA.begin();
}
