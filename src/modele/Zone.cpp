#include "Zone.h"

Zone::Zone(byte id, String nomZone) {
    DEBUGLN(F("Zone - +Constructeur"));
    idZone = id;
    nom = nomZone;

    DEBUGLN(F("Zone - -Constructeur"));
}

void Zone::setTempConsigne(float valeur) {
    DEBUGLN(F("Zone - +setTempConsigne"));
    tempConsigne = valeur;
    DEBUGLN(F("Zone - -setTempConsigne"));
}

void Zone::setTempAmbiance(float valeur) {
    DEBUGLN(F("Zone - +setTempAmbiance"));
    tempAmbiance = valeur;
    DEBUGLN(F("Zone - -setTempAmbiance"));
}
