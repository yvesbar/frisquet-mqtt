#include "Zone.h"


Zone::Zone(byte id, String nomZ) {
    idZone = id;
    nom = nomZ;
}

void Zone::setTempConsigne(uint8_t valeur) {
    if (valeur != tempConsigne) {
        tempConsigne = valeur;
    }
}

void Zone::setTempAmbiance(uint8_t valeur) {
    if (valeur != tempAmbiance) {
        tempAmbiance = valeur;
    }
}

void Zone::setMode(uint8_t valeur) {
    if (valeur != mode) {
        mode = valeur;
    }
}
