#include "Zone.h"

Zone::Zone(byte id, String nomZone) {
    DEBUGLN(F("Zone - +Constructeur"));
    idZone = id;
    nom = nomZone;

    DEBUGLN(F("Zone - -Constructeur"));
}

void Zone::setTempConsigne(float valeur) {
    DEBUGLN(F("Zone - +setTempConsigne"));

    if (valeur != tempConsigne) {
        tempConsigne = valeur;
    }
    DEBUGLN(F("Zone - -setTempConsigne"));
}

void Zone::setTempAmbiance(float valeur) {
    DEBUGLN(F("Zone - +setTempAmbiance"));

    if (valeur != tempAmbiance) {
        tempAmbiance = valeur;
    }
    DEBUGLN(F("Zone - -setTempAmbiance"));
}

void Zone::setMode(float valeur) {
    DEBUGLN(F("Zone - +setMode"));

    if (valeur != mode) {
        mode = valeur;
    }

    DEBUGLN(F("Zone - -setMode"));
}
