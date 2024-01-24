#ifndef __ZONE_H_
#define __ZONE_H_

#define ZONE_MODE_CONFORT 0x01
#define ZONE_MODE_ECO 0x02
#define ZONE_MODE_HORS_GEL 0x03

#include <Arduino.h>

class Zone {
    public:
        /* Température de consigne */
        uint8_t getTempConsigne() const { return tempConsigne; }
        void setTempConsigne(uint8_t valeur);

        /* Température d'Ambiance */
        uint8_t getTempAmbiance() const { return tempAmbiance; }
        void setTempAmbiance(uint8_t valeur);

        /* Mode */
        uint8_t getMode() const { return mode; }
        void setMode (uint8_t valeur);

        /* indique si une mise à jour a été faite sur la zone */
        bool isMiseAJour();

    private:
        uint8_t tempConsigne;
        bool tempConsigneMaj;

        uint8_t tempAmbiance;
        bool tempAmbianceMaj;

        uint8_t mode;
        bool modeMaj;
};

#endif //__ZONE_H_