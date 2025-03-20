#ifndef __ZONE_H_
#define __ZONE_H_

#define ZONE_MODE_CONFORT 0x01
#define ZONE_MODE_ECO 0x02
#define ZONE_MODE_HORS_GEL 0x03

#include <Arduino.h>
#include <string>

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
        uint8_t getTempConsigne() const { return tempConsigne; }
        void setTempConsigne(uint8_t valeur);
        
        /* Température d'Ambiance */
        uint8_t getTempAmbiance() const { return tempAmbiance; }
        void setTempAmbiance(uint8_t valeur);
        
        /* Mode */
        uint8_t getMode() const { return mode; }
        void setMode (uint8_t valeur);
        
        //TODO ajouter le constructeur qui prend en param un ID de zone
        
    private:
        //Identifiant de zone
        byte idZone;
        String nom;

        uint8_t tempConsigne;
        uint8_t tempAmbiance;
        uint8_t mode;
};

#endif //__ZONE_H_