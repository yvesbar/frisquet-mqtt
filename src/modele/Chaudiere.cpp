#include "Chaudiere.h"

Chaudiere::Chaudiere() {
    zone1 = new Zone(Zone::ZONE1_ID, ZONE1_NOM);

    if (ZONE2_ACTIF) {
        zone2 = new Zone(Zone::ZONE2_ID, ZONE2_NOM);
    }

    if (ZONE3_ACTIF) {
        zone3 = new Zone(Zone::ZONE3_ID, ZONE3_NOM);
    }
}


void Chaudiere::setTempExterieure(int value) {
    if (value != tempExterieure) {
        tempExterieure = value;
        tempExterieurMaj = true;
    }
}

bool Chaudiere::isMiseAJour() {
    return ((zone1 == nullptr) ? false : zone1->isMiseAJour())
             && ((zone2 == nullptr) ? false : zone2->isMiseAJour())
             && ((zone3 == nullptr) ? false : zone3->isMiseAJour())
             && tempExterieurMaj;
}