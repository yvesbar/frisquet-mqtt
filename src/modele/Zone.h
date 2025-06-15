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
        enum Mode {
            JOUR,
            NUIT,
            HORS_GEL
        };

        /* Constructeur avec id de zone */
        Zone(byte, String);

        /* identifiant de la zone */
        byte getId() const { return idZone; };
        String getNom() const { return nom ; };

        /* Température de consigne */
        float getTempConsigne() const { return tempConsigne; }
        void setTempConsigne(float valeur) { tempConsigne = valeur; }
        
        /* Température d'Ambiance */
        float getTempAmbiance() const { return tempAmbiance; }
        void setTempAmbiance(float valeur){ tempAmbiance = valeur; }
        
        /* Mode */
        Mode getMode() const { return mode; }
        void setMode (Mode valeur) { mode = valeur; }

        /* Température de départ */
        float getTempDepart() const { return tempDepart; }
        void setTempDepart(float valeur) { tempDepart = valeur; }
        
    private:
        //Identifiant de zone
        byte idZone;
        String nom;

        float tempConsigne;
        float tempAmbiance;
        float tempDepart;
        Mode mode;
};

#endif //__ZONE_H_