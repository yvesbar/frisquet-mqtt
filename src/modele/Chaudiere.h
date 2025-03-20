#ifndef __CHAUDIERE_H_
#define __CHAUDIERE_H_

#include <Arduino.h>
#include "Zone.h"
#include "config.h"


class Chaudiere {
    public:
        Chaudiere();
        
        Zone *getZone1() const { return zone1; };
        Zone *getZone2() const { return zone2; };
        Zone *getZone3() const { return zone3; };

        /* températur extérieure */
        void setTempExterieure(int value);

    private:
        Zone *zone1;
        Zone *zone2;
        Zone *zone3;

        uint8_t tempExterieure;

        Zone* getZoneById(int zoneId);
};

#endif  //__CHAUDIERE_H_