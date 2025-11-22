#include "Chaudiere.h"

Chaudiere& Chaudiere::getInstance() {
    static Chaudiere instance; // Instance unique de la classe
    return instance;
}

Chaudiere::Chaudiere() {
    DEBUGLN(F("Chaudiere - Constructeur"));
}

/**
 * Ajoute une zone dynamiquement à la chaudière
 */
Zone* Chaudiere::addZone(byte id) {
    Zone* zone = nullptr;
    switch (id)  {
        case Zone::ZONE1_ID:
            zone = new Zone(id, ZONE1_NOM);
            zones[id] = zone;
            break;
        case Zone::ZONE2_ID:
            zone = new Zone(id, ZONE2_NOM);
            zones[id] = zone;
            break;
        case Zone::ZONE3_ID:
            zone = new Zone(id, ZONE3_NOM);
            zones[id] = zone;
            break;
        default:
            DEBUG(F("Zone inconnue : "));
            DEBUGLN(id);
            break;
    }

    return zone;
}

Zone* Chaudiere::getZoneById(byte id) {
    //Les zones sont créées dynamiquement à la première réception d'une trame les concernant
    return zones[id]; 
}

void Chaudiere::setTempExterieure(float value) {
    tempExterieure = value;
}

void Chaudiere::setTempCorpsDeChauffe(float value) {
    tempCorpsDeChauffe = value;
}

void Chaudiere::setTempECS(float value) {
    tempECS = value;
}