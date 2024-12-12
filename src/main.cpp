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
volatile bool receivedFlag = false;
#define SS GPIO_NUM_8
#define RST_LoRa GPIO_NUM_12
#define BUSY_LoRa GPIO_NUM_13
#define DIO0 GPIO_NUM_14

SX1262 radio = new Module(SS, DIO0, RST_LoRa, BUSY_LoRa);
Preferences preferences;
unsigned long lastTxExtSonTime = 30000;           // Variable dernière transmission sonde ajout de 30 sec d'intervale pour ne pas envoyé en meme temps que le connect
const unsigned long txExtSonInterval = 600000; // Interval de transmission en millisecondes (10 minutes)
unsigned long lastConMsgTime = 0;
const unsigned long conMsgInterval = 600000; // 10 minutes
unsigned long lastconMsgInterval = 0; // pour l'interval de 1 seconde entre les messages du Con a cha
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
byte conMsgNum = 0x03;
int counter = 0;
uint8_t custom_network_id[4];
uint8_t custom_extSon_id;
uint8_t custom_friCon_id;
float temperatureValue;
float temperatureconsValue;
float extSonVal;
WiFiClient espClient;
PubSubClient client(espClient);
bool waitingForResponse = false;
unsigned long startWaitTime = 0;
unsigned long lastTxModeTime = 0;
const unsigned long maxWaitTime = 360000; // 6 minutes en ms
const unsigned long retryInterval = 2500; // 2.5 secondes en ms
const char hexDigits[] = "0123456789ABCDEF";
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
byte TxByteArrCon1[10] = {0x80, 0x7e, 0x21, 0xE0, 0x01, 0x03, 0xA0, 0x2B, 0x00, 0x04};                                       // message A0	2B	00	04 connect to chaudiere
byte TxByteArrCon2[10] = {0x80, 0x7e, 0x21, 0xE0, 0x01, 0x03, 0x79, 0xE0, 0x00, 0x1C};                                       // message 79	E0	00	1C connect to chaudiere
byte TxByteArrCon3[10] = {0x80, 0x7e, 0x21, 0xE0, 0x01, 0x03, 0x7A, 0x18, 0x00, 0x1C};                                       // message 7A	18	00	1C connect to chaudiere
byte TxByteArrCon4[10] = {0x80, 0x7e, 0x21, 0xE0, 0x01, 0x03, 0x79, 0xFC, 0x00, 0x1C};                                       // message 79	FC	00	1C connect to chaudiere
byte TxByteArrConRep[49] = {
    0x80, 0x7E, 0x39, 0x18, 0x88, 0x17, 0x2A, 0x91, 0x6E, 0x1E, 0x05, 0x21, 0x00, 0x00, 0xE0, 0xFF,
    0xFF, 0xFF, 0x1F, 0x00, 0xE0, 0xFF, 0xFF, 0xFF, 0x1F, 0x00, 0xE0, 0xFF, 0xFF, 0xFF, 0x1F, 0x00,
    0xE0, 0xFF, 0xFF, 0xFF, 0x1F, 0x00, 0xE0, 0xFF, 0xFF, 0xFF, 0x1F, 0x00, 0xE0, 0xFF, 0xFF, 0xFF,
    0x1F};
byte TxByteArrConMod[63] = {
    0x80, 0x7E, 0x39, 0x18, 0x08, 0x17, 0xA1, 0x54, 0x00, 0x18, 0xA1, 0x54, 0x00, 0x18, 0x30, 0x91,
    0x6E, 0x1E, 0x08, 0x21, 0x00, 0x00, 0xE0, 0xFF, 0xFF, 0xFF, 0x1F, 0x00, 0xE0, 0xFF, 0xFF, 0xFF,
    0x1F, 0x00, 0xE0, 0xFF, 0xFF, 0xFF, 0x1F, 0x00, 0xE0, 0xFF, 0xFF, 0xFF, 0x1F, 0x00, 0xE0, 0xFF,
    0xFF, 0xFF, 0x1F, 0x00, 0xE0, 0xFF, 0xFF, 0xFF, 0x1F, 0x00, 0xE0, 0xFF, 0xFF, 0xFF, 0x1F};
//****************************************************************************
// Array pour envoyé les trames au connect avec un loop qui espace
int conMsgIndex = 0;
int conMsgToSendCount = 0;

byte *conMsgArrays[] = {
    TxByteArrCon1,
    TxByteArrCon2,
    TxByteArrCon3,
    TxByteArrCon4};
const int conMsgCount = 4;
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
void updateDisplay()
{
  if (tempAmbianteChanged || tempExterieureChanged || tempConsigneChanged || modeFrisquetChanged)
  {
    Heltec.display->clear();
    Heltec.display->drawString(0, 0, "Net: " + byteArrayToHexString(custom_network_id, sizeof(custom_network_id)));
    Heltec.display->drawString(0, 11, "SonID: " + byteArrayToHexString(&custom_extSon_id, 1) + " ConID: " + byteArrayToHexString(&custom_friCon_id, 1));
    Heltec.display->drawString(0, 22, "T° Amb: " + tempAmbiante + "°C " + "T° Ext: " + tempExterieure + "°C");
    Heltec.display->drawString(0, 33, "T° Con: " + tempConsigne + "°C" + "Mode: " + modeFrisquet);

    Heltec.display->display();
    tempAmbianteChanged = false;
    tempExterieureChanged = false;
    tempConsigneChanged = false;
    modeFrisquetChanged = false;
  }
  else if (assSonFrisquetChanged || assConFrisquetChanged)
  {
    Heltec.display->clear();
    Heltec.display->drawString(0, 0, "Net: " + byteArrayToHexString(custom_network_id, sizeof(custom_network_id)));
    Heltec.display->drawString(0, 11, "SonID: " + byteArrayToHexString(&custom_extSon_id, 1) + " ConID: " + byteArrayToHexString(&custom_friCon_id, 1));
    Heltec.display->drawString(0, 22, "Ass. sonde en cours: " + assSonFrisquet);
    Heltec.display->drawString(0, 33, "Ass. connect en cours: " + assConFrisquet);

    Heltec.display->display();
    assSonFrisquetChanged = false;
    assConFrisquetChanged = false;
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
void handleModeChange(const char *newMode)
{
  // Déterminer la valeur du mode à transmettre avec un switch
  uint8_t modeValue1 = 0x00; // Par défaut, valeur invalide
  uint8_t modeValue2 = 0x00;
  bool modeValid = true;

  switch (newMode[0]) // Utiliser le premier caractère pour optimiser
  {
  case 'A': // "Auto"
    modeValue1 = 0x05;
    modeValue2 = 0x21;
    break;
  case 'C': // "Confort"
    modeValue1 = 0x06;
    modeValue2 = 0x21;
    break;
  case 'R': // "Réduit"
    modeValue1 = 0x07;
    modeValue2 = 0x21;
    break;
  case 'H': // "Hors gel"
    modeValue1 = 0x08;
    modeValue2 = 0x21;
    break;
  default:
    modeValid = false; // Mode non valide
    break;
  }

  if (!modeValid)
  {
    Serial.println("Mode non reconnu !");
    return; // Sortir si le mode est inconnu
  }

  // Assigner la valeur du mode à TxByteArrConMod
  TxByteArrConMod[3] = conMsgNum;
  TxByteArrConMod[18] = modeValue1;
  TxByteArrConMod[19] = modeValue2;

  conMsgNum += 1;
  // Si conMsgNum dépasse 255, le remettre à une valeur valide (par exemple, 3)
  if (conMsgNum > 0xFF)
  {
    conMsgNum = 0x03; // Recommencer à 3 pour maintenir le décalage
  }
  // Envoyer la chaîne via LoRa
  int state = radio.transmit(TxByteArrConMod, sizeof(TxByteArrConMod));
  if (state == RADIOLIB_ERR_NONE)
  {
    waitingForResponse = true;
  }
  else
  {
    Serial.println("Erreur lors de l'envoi du mode !");
  }
  state = radio.startReceive();
    if (state == RADIOLIB_ERR_NONE)
    {
      Serial.println(F("startreceive success!"));
    }
    else
    {
      Serial.print(F("failed, code "));
      Serial.println(state);
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
      handleModeChange(message);
      startWaitTime = millis(); // On note le début de l’attente
      // client.publish("homeassistant/select/frisquet/mode/state", message);
    }
  }
  else if (strcmp(topic, ASS_SON_TOPIC) == 0)
  {
    if (assSonFrisquet != String(message))
    {
      assSonFrisquet = String(message);
      assSonFrisquetChanged = true;
      client.publish("homeassistant/switch/frisquet/asssonde/state", message);
      if (assSonFrisquet == "OFF")
      {
        initNvs();
      }
    }
  }
  else if (strcmp(topic, ASS_CON_TOPIC) == 0)
  {
    if (assConFrisquet != String(message))
    {
      assConFrisquet = String(message);
      assConFrisquetChanged = true;
      client.publish("homeassistant/switch/frisquet/assconnect/state", message);
      if (assConFrisquet == "OFF")
      {
        initNvs();
      }
    }
  }
  else if (strcmp(topic, RES_NVS_TOPIC) == 0)
  {
    if (eraseNvsFrisquet != String(message))
    {
      eraseNvsFrisquet = String(message);
      client.publish("homeassistant/switch/frisquet/erasenvs/state", message);
    }
  }
}
//****************************************************************************
void connectToMqtt()
{
  while (!client.connected())
  {
    Serial.println(F("Connecting to MQTT..."));
    if (client.connect("ESP32 Frisquet", mqttUsername, mqttPassword))
    {
    }
    else
    {
      Serial.print(F("Failed to connect MQTT, rc="));
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
  // Serial.print("Sonde transmit: ");
  //for (int i = 0; i < sizeof(TempExTx); i++)
  //{
  //  Serial.printf("%02X ", TempExTx[i]);
  //  Serial.print(" ");
  //}
  Serial.println();
  // Transmettre la chaine TempExTx
  int state = radio.transmit(TempExTx, sizeof(TempExTx));
  if (state == RADIOLIB_ERR_NONE)
  {
    Serial.println(F("Transmission temp. ext. réussie"));
    state = radio.startReceive();
    if (state == RADIOLIB_ERR_NONE)
    {
      Serial.println(F("startreceive success!"));
    }
    else
    {
      Serial.print(F("failed, code "));
      Serial.println(state);
    }
  }
  else
  {
    Serial.println(F("Erreur trans. temp ext."));
    Serial.println(state);
  }
}
//****************************************************************************
void txfriConMsg()
{
  // Insérer conMsgNum et custom_friCon_id dans les trames à envoyer
if (millis() - lastconMsgInterval >= 1000) // 4000 ms = 4 secondes
  {
  conMsgArrays[conMsgIndex][3] = conMsgNum;
  conMsgArrays[conMsgIndex][2] = custom_friCon_id;
  // envoi de la trame
  int state = radio.transmit(conMsgArrays[conMsgIndex], 10); // 10 est la taille de chaque tableau TxByteArrConX
  if (state == RADIOLIB_ERR_NONE)
  {
    Serial.println(F("Transmission msg. Con. réussie"));

    state = radio.startReceive();
    if (state == RADIOLIB_ERR_NONE)
    {
      Serial.println(F("startreceive success!"));
    }
    else
    {
      Serial.print(F("failed, code "));
      Serial.println(state);
    }
  }
  else
  {
    Serial.println(F("Erreur lors de la transmission Con"));
  }

  // Incrémenter conMsgNum de 4 et gérer le débordement
  conMsgNum += 4;
  // Si conMsgNum dépasse 255, le remettre à une valeur valide
  if (conMsgNum > 0xFF)
  {
    conMsgNum = 0x03; // Recommencer à 3 pour maintenir le décalage
  }

  // Préparer la prochaine trame
  conMsgIndex++;
  conMsgToSendCount--;
  lastconMsgInterval = millis();
  }
  else
  {
    // Attente si le délai de 1 secondes n'est pas encore atteint
    Serial.println(F("Attente avant la prochaine transmission..."));
  }
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
    // Serial.printf("RECEIVED [%2d] : ", len);
    // for (int i = 0; i < len; i++)
    //  Serial.printf("%02X ", byteArr[i]);
    // Serial.println();

    // Vérifier la longueur attendue (ici 11 comme pour l'extSon) ou adapter selon le device
    if (len == 11)
    {
      // Adapter le payload à partir du message reçu
      deviceTxArr[2] = byteArr[2];
      deviceTxArr[3] = byteArr[3];
      deviceTxArr[4] = byteArr[4] | 0x80; // Ajouter 0x80 au 5eme byte
      deviceTxArr[5] = byteArr[5];
      deviceTxArr[6] = byteArr[6];

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
bool assExtSon()
{
  bool result = associateDevice(
      TxByteArr, sizeof(TxByteArr),
      def_Network_id,
      "son_id",
      ASS_SON_TOPIC,
      "homeassistant/switch/frisquet/asssonde/state");

  if (result)
  {
    Serial.println("Association de la sonde réussie !");
    assSonFrisquet = "OFF";
  }
  else
  {
    Serial.println("Echec de l'association de la sonde !");
    // Action en cas d'échec, ex. retenter ou notifier
  }
  return result;
}
//****************************************************************************
bool assFriCon()
{
  bool result = associateDevice(
      TxByteArrFriCon, sizeof(TxByteArrFriCon),
      def_Network_id,
      "con_id",
      ASS_CON_TOPIC,
      "homeassistant/switch/frisquet/assconnect/state");

  if (result)
  {
    Serial.println("Association du Frisquet Connect réussie !");
    assConFrisquet = "OFF";
  }
  else
  {
    Serial.println("Echec de l'association du Frisquet Connect !");
  }
  return result;
}
//****************************************************************************
void initOTA();
//****************************************************************************
void setFlag(void)
{
  // we got a packet, set the flag
  receivedFlag = true;
}
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
  // Heltec.display->flipScreenVertically();
  Heltec.display->setFont(ArialMT_Plain_10);
  Heltec.display->clear();
  Heltec.display->drawXbm(0, 0, 128, 64, myLogo);
  Heltec.display->display();

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

  // set the function that will be called
  // when new packet is received
  radio.setPacketReceivedAction(setFlag);

  // start listening for Radio packets
  int state = radio.beginFSK();
  state = radio.setFrequency(868.96);
  state = radio.setBitRate(25.0);
  state = radio.setFrequencyDeviation(50.0);
  state = radio.setRxBandwidth(250.0);
  state = radio.setPreambleLength(4);
  state = radio.setSyncWord(custom_network_id, sizeof(custom_network_id));
  Serial.print(F("[SX1262] Starting to listen ... "));
  state = radio.startReceive();
  if (state == RADIOLIB_ERR_NONE)
  {
    Serial.println(F("success!"));
  }
  else
  {
    Serial.print(F("failed, code "));
    Serial.println(state);
  }

  preferences.end(); // Fermez la mémoire NVS ici
}
//****************************************************************************
void adaptMod(uint8_t modeValue)
{
  const char *topic = "homeassistant/select/frisquet/mode/state"; // Topic MQTT pour le mode
  const char *mode;

  // Traduire la valeur en un mode texte
  switch (modeValue)
  {
  case 0x05:
    mode = "Auto";
    break;
  case 0x06:
    mode = "Confort";
    break;
  case 0x07:
    mode = "Réduit";
    break;
  case 0x08:
    mode = "Hors gel";
    break;
  default:
    mode = "Inconnu"; // Valeur par défaut si non reconnue
    break;
  }

  // Publier le mode sur le topic MQTT
  if (client.publish(topic, mode))
  {
    Serial.println("Mode mis a jour sur MQTT");
  }
  else
  {
    Serial.println("Erreur lors de la publication du mode !");
  }
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
  else if (len == 63)
  {
    if (byteArr[0] == 0x7e && byteArr[1] == 0x80 && byteArr[4] == 0x08 && byteArr[5] == 0x17)
    {
      TxByteArrConRep[3] = byteArr[3];
      memcpy(&TxByteArrConRep[7], &byteArr[15], 41); // Copie 41 octets depuis byteArr[15] dans TxByteArrConRep[7]
      // Envoi de la chaine d'association
      int State = radio.transmit(TxByteArrConRep, sizeof(TxByteArrConRep));
      if (State == RADIOLIB_ERR_NONE)
      {
        //  Appeler adaptMod avec la valeur extraite de byteArr[10]
        uint8_t modeValue = TxByteArrConRep[10];
        adaptMod(modeValue);
      }
      else
      {
        Serial.println("Erreur lors de la transmission !");
      }
    }
  }
  else if (len == 55 && waitingForResponse)
  {
    waitingForResponse = false;
  }
  int pos = 0;

  // Convertir chaque octet en deux caractères hexadécimaux
  for (int i = 0; i < len; i++)
  {
    uint8_t b = byteArr[i];
    // Premier nibble (4 bits)
    message[pos++] = hexDigits[b >> 4];
    // Deuxième nibble
    message[pos++] = hexDigits[b & 0x0F];
    // Ajouter un espace séparateur
    message[pos++] = ' ';
    // Vérifier qu'on ne dépasse pas la taille du buffer
    if (pos >= (int)(sizeof(message) - 2))
      break;
  }

  // Si on a ajouté un espace en trop à la fin, on l'enlève
  if (pos > 0 && message[pos - 1] == ' ')
  {
    pos--;
  }

  // Terminer la chaîne
  message[pos] = '\0';
  if (!client.publish("homeassistant/sensor/frisquet/payload/state", message))
  {
    Serial.println(F("Failed to publish Payload to MQTT"));
  }
  Serial.println(F(""));
}
//****************************************************************************
void loop()
{
  // check if the flag is set
  if (receivedFlag)
  {
    // reset flag
    receivedFlag = false;

    // you can read received data as an Arduino String
    byte byteArr[RADIOLIB_SX126X_MAX_PACKET_LENGTH];
    int state = radio.readData(byteArr, 0);
    if (state == RADIOLIB_ERR_NONE)
    {
      // packet was successfully received
      Serial.println(F("[SX1262] Received packet!"));

      // print data of the packet
      Serial.print(F("[SX1262] Data:\t\t"));
      int len = radio.getPacketLength();
      handleRadioPacket(byteArr, len);

      // print RSSI (Received Signal Strength Indicator)
      //Serial.print(F("[SX1262] RSSI:\t\t"));
      //Serial.print(radio.getRSSI());
      //Serial.println(F(" dBm"));

      // print SNR (Signal-to-Noise Ratio)
      //Serial.print(F("[SX1262] SNR:\t\t"));
      //Serial.print(radio.getSNR());
      //Serial.println(F(" dB"));

      // print frequency error
      //Serial.print(F("[SX1262] Frequency error:\t"));
      //Serial.print(radio.getFrequencyError());
      //Serial.println(F(" Hz"));
    }
    else if (state == RADIOLIB_ERR_CRC_MISMATCH)
    {
      // packet was received, but is malformed
      Serial.println(F("CRC error!"));
    }
    else
    {
      // some other error occurred
      Serial.print(F("failed, code "));
      Serial.println(state);
    }
  }
  // byte byteArr[RADIOLIB_SX126X_MAX_PACKET_LENGTH];
  // int state = radio.receive(byteArr, 0);
  // if (state == RADIOLIB_ERR_NONE)
  //{
  //  int len = radio.getPacketLength();
  //  handleRadioPacket(byteArr, len);
  //}
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
    // Vérifier si c'est le moment de démarrer l'envoi des 4 trames
    if (currentTime - lastConMsgTime >= conMsgInterval)
    {
      // Vérifier si custom_friCon_id n'est pas égal à 0 byte
      if (memcmp(custom_network_id, "\xFF\xFF\xFF\xFF", 4) != 0 && custom_friCon_id != 0x00)
      {
        // On remet à zéro l'index et on indique qu'il faut envoyer 4 trames
        conMsgIndex = 0;
        conMsgToSendCount = 4;
        lastConMsgTime = currentTime; // on remet le timer à zéro
      }
      else
      {
        Serial.println(F("Id frisquet connect non connue"));
      }
    }

    // Si on a des trames à envoyer depuis connect
    if (conMsgToSendCount > 0)
    {
      txfriConMsg(); // Appeler la fonction pour transmettre les données
    }
    if (!client.connected())
    {
      connectToMqtt();
    }
    ArduinoOTA.handle();

    // Compteur pour limiter la déclaration des topic
    if (counter >= 100000)
    {
      connectToTopic();
      counter = 0;
    }
    counter++;
    // Changement de mode depuis HA
    if (waitingForResponse)
    {
      unsigned long currentTime = millis();

      // Vérification du timeout des 4 minutes
      if (currentTime - startWaitTime >= maxWaitTime)
      {
        waitingForResponse = false;
        Serial.println("Timeout mode");
      }
      else
      {
        // Réenvoi toutes les 2 secondes
        if (currentTime - lastTxModeTime >= retryInterval)
        {
          handleModeChange(modeFrisquet.c_str());
          // int state = radio.transmit(TxByteArrConMod, sizeof(TxByteArrConMod));
          lastTxModeTime = currentTime;
        }
      }
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
