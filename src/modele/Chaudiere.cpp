#include "Chaudiere.h"

Chaudiere::Chaudiere() {
    DEBUGLN(F("Chaudiere - Création zone 1"));
    zone1 = new Zone(Zone::ZONE1_ID, ZONE1_NOM);

    if (ZONE2_ACTIF) {
        DEBUGLN(F("Chaudiere - Création zone 2"));
        zone2 = new Zone(Zone::ZONE2_ID, ZONE2_NOM);
    }

    if (ZONE3_ACTIF) {
        DEBUGLN(F("Chaudiere - Création zone 3"));
        zone3 = new Zone(Zone::ZONE3_ID, ZONE3_NOM);
    }
}

void Chaudiere::setTempExterieure(uint8_t value) {
    if (value != tempExterieure) {
        tempExterieure = value;
    }
}

void Chaudiere::setTempCorpsDeChauffe(uint8_t value) {
    if (value != tempCorpsDeChauffe) {
        tempCorpsDeChauffe = value;
    }
}

Zone* Chaudiere::getZoneById(int zoneId) {
    if (zone1 != nullptr && zone1->getId() == zoneId) {
        return zone1;
    }
    if (zone2 != nullptr && zone2->getId() == zoneId) {
        return zone2;
    }
    if (zone3 != nullptr && zone3->getId() == zoneId) {
        return zone3;
    }
    return nullptr;
}