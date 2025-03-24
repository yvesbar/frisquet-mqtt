#include "Chaudiere.h"

Chaudiere::Chaudiere() {
    DEBUGLN(F("Chaudiere - Constructeur"));
}

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
    DEBUG(F("Chaudiere - getZoneById : "));
    if (zones.find(id) == zones.end()) {
        DEBUG(F("Chaudiere - Création d'une nouvelle zone avec l'ID : "));
        DEBUGLN(id);
        
    }
    return zones[id]; // Retourne la zone existante ou nouvellement créée
}

void Chaudiere::setTempExterieure(float value) {
    DEBUG(F("Chaudiere - setTempExterieure : "));
    DEBUGLN(value);
    if (value != tempExterieure) {
        tempExterieure = value;
    }
}

void Chaudiere::setTempCorpsDeChauffe(float value) {
    DEBUG(F("Chaudiere - setTempCorpsDeChauffe : "));
    DEBUGLN(value);
    if (value != tempCorpsDeChauffe) {
        tempCorpsDeChauffe = value;
    }
}