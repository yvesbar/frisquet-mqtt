#ifndef __CHAUDIERE_H_
#define __CHAUDIERE_H_

#include <Arduino.h>
#include "Zone.h"
#include "config.h"
#include "../debug.h"

/*
    Classe représentant la chaudière frisquet
    TODO - Les fonctionnalités de la chaudière sont ajoutées au fur et à mesure de la réception des trames
*/ 
class Chaudiere {
    public:
        Chaudiere();
        
        Zone *getZone1() const { return zone1; };
        Zone *getZone2() const { return zone2; };
        Zone *getZone3() const { return zone3; };

        /* températur extérieure */
        void setTempExterieure(uint8_t value);
        void setTempCorpsDeChauffe(uint8_t value);

    private:
        Zone *zone1 = nullptr;
        Zone *zone2 = nullptr;
        Zone *zone3 = nullptr;

        uint8_t tempExterieure;
        uint8_t tempCorpsDeChauffe;

        //consoGazChauffage
        //consoGazECS
        //

        Zone* getZoneById(int zoneId);
};

#endif  //__CHAUDIERE_H_