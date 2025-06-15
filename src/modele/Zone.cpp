#include "Zone.h"

Zone::Zone(byte id, String nomZone) {
    DEBUGLN(F("Zone - +Constructeur"));
    idZone = id;
    nom = nomZone;

    DEBUGLN(F("Zone - -Constructeur"));
}
