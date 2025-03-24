#ifndef __ZONE_H_
#define __ZONE_H_

#define ZONE_MODE_CONFORT 0x01
#define ZONE_MODE_ECO 0x02
#define ZONE_MODE_HORS_GEL 0x03

#include <Arduino.h>
#include <string>
#include "../debug.h"

class Zone {
    public:
        static const byte ZONE1_ID = 0x08;
        static const byte ZONE2_ID = 0x09;
        static const byte ZONE3_ID = 0x0a;

        /* Constructeur avec id de zone */
        Zone(byte, String);

        /* identifiant de la zone */
        byte getId() const { return idZone; };
        String getNom() const { return nom ; };

        /* Température de consigne */
        float getTempConsigne() const { return tempConsigne; }
        void setTempConsigne(float valeur);
        
        /* Température d'Ambiance */
        float getTempAmbiance() const { return tempAmbiance; }
        void setTempAmbiance(float valeur);
        
        /* Mode */
        float getMode() const { return mode; }
        void setMode (float valeur);
        
        //TODO ajouter le constructeur qui prend en param un ID de zone
        
    private:
        //Identifiant de zone
        byte idZone;
        String nom;

        float tempConsigne;
        float tempAmbiance;
        uint8_t mode;
};

#endif //__ZONE_H_