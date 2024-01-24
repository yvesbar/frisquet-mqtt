#include "Zone.h"

void Zone::setTempConsigne(uint8_t valeur) {
    if (valeur != tempConsigne) {
        tempConsigne = valeur;
        tempConsigneMaj = true;
    }
}

void Zone::setTempAmbiance(uint8_t valeur) {
    if (valeur != tempAmbiance) {
        tempAmbiance = valeur;
        tempAmbianceMaj = true;
    }
}

void Zone::setMode(uint8_t valeur) {
    if (valeur != mode) {
        mode = valeur;
        modeMaj = true;
    }
}

bool Zone::isMiseAJour() {
    return tempAmbianceMaj && tempConsigneMaj && modeMaj;
}
