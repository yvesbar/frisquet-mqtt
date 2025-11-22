# Frisquet MQTT - Instructions Copilot

## Vue d'ensemble de l'architecture

Système ESP32 surveillant une chaudière Frisquet via radio FSK (868.96 MHz), publiant les données vers MQTT/Home Assistant.

**Origine du projet :** Refonte sous forme de classes du projet original de burnallover. Le code original monolithique est disponible dans `src/main.cpp.old` pour référence et comparaison des fonctionnalités.

**Composants principaux :**
- `ModuleHeltec` : Abstraction matérielle (WiFi, LoRa SX1262, OLED, OTA)
- `VisioConnectSub` : Décodeur de trames radio (trames 17/23/49/63 octets)
- `MqttPub` : Éditeur MQTT avec filtrage anti-spam
- `Zone` : Modèle de zone de chauffage (temp, consigne, mode)
- `Chaudiere` : Gestionnaire singleton de l'état chaudière

## Modèles clés

### Convention de nommage des méthodes
- Noms français sauf préfixes techniques : `get/set/publish` + français
- Exemple : `publishZoneTempConsigne()`, `getTempAmbiance()`

### Configuration
- `config.h` (copie de `config.h.example`) contient tous les paramètres utilisateur
- Config IP statique via booléen `WIFI_IP_STATIQUE_ACTIVE` + tableaux IP
- Noms de zones, broker MQTT, network_id radio tous configurables

### Traitement des trames radio
- La longueur de trame détermine le handler : `lireTrame17/23/49/63()`
- Encodage température : 2 octets → `Utils::bytesToTemperature()` (division par 10)
- **Critique** : Utiliser `float` pas `int` pour températures pour éviter troncature 19.5→19

### Publication MQTT
- Centralisée via `publishFloatRetained(topic, value, decimals, retain, filter)`
- Filtre anti-spam : publie seulement si ΔT ≥ seuil OU intervalle temps dépassé
- Auto-découverte Home Assistant via `deployConfHAZone()`
- Topics : pattern `frisquet/{zone}/{métrique}`

### Gestion mémoire
- Limitée à ~300Ko RAM - éviter concaténation String dans boucles
- Utiliser buffers `char[]` pour topics, `snprintf()` pour formatage
- Cache `TopicState` trace dernières valeurs publiées pour filtrage

## Flux de développement

```bash
# Construction et upload
platformio run
platformio run -t upload --upload-port /dev/cu.usbserial-0001

# Monitoring série
platformio device monitor
```

## Tâches courantes

### Ajout nouveau capteur température
1. Créer getter dans `Zone` ou `Chaudiere`
2. Ajouter topic MQTT dans `MqttPub` suivant le pattern
3. Parser depuis trame radio dans méthode `lireTrame*()` appropriée
4. Utiliser `publishFloatRetained()` avec filtrage

### Décodage de trame
- Toujours utiliser `float temperatureValue = Utils::bytesToTemperature(trame[X], trame[Y])`
- Vérifier changement valeur avant publication : `if (zone->getTemp() != temperatureValue)`
- Mettre à jour modèle puis publier : `zone->setTemp(value); mqttPub->publishTemp(zone, value)`

### Patterns de debug
- Utiliser `DEBUGLN(F("message"))` pour stockage chaîne en flash
- Trames radio loggées en hex : `Serial.printf("RECEIVED [%2d] : ", len)`
- Info réseau loggée à la connexion WiFi (IP, MAC)

## Fichiers critiques
- `src/config.h.example` : Toutes les options de configuration avec exemples
- `src/interfaces/VisioConnectSub.cpp` : Logique de parsing des trames
- `src/interfaces/MqttPub.cpp` : Publication avec anti-spam
- `src/modele/Zone.h` : Structure de données zone de chauffage principale
- `doc/` : Documentation technique et exemples de trames radio pour comprendre le protocole Frisquet


# Mon installation
sur mon installation j'ai une Chaudière Frisquet Hydromotrix 25kW qui gère une zone 1.
Un module H qui gère une zone 2 (plancher chauffant) et un capteur de température extérieur.
Je n'ai pas de zone 3.
Ma chaudière fait aussi l'ECS (Eau Chaude Sanitaire).
