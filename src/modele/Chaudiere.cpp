#include "chaudiere.h"

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