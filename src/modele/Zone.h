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
            AUTO,
            CONFORT,
            REDUIT,
            HORS_GEL
        };
        enum JourSemaine {
            DIMANCHE = 0,
            LUNDI = 1,
            MARDI = 2,
            MERCREDI = 3,
            JEUDI = 4,
            VENDREDI = 5,
            SAMEDI = 6
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
        
        /* Température de confort */
        float getTempConfort() const { return tempConfort; }
        void setTempConfort(float valeur) { tempConfort = valeur; }

        /* Température réduit */
        float getTempReduit() const { return tempReduit; }
        void setTempReduit(float valeur) { tempReduit = valeur; }

        /* Température hors gel */
        float getTempHorsGel() const { return tempHorsGel; }
        void setTempHorsGel(float valeur) { tempHorsGel = valeur; }

        /* Programmation hebdomadaire (6 octets par jour, 7 jours) */
        void setProgrammationJour(JourSemaine jour, const uint8_t* prog) {
            memcpy(programmation[jour], prog, 6);
        }
        const uint8_t* getProgrammationJour(JourSemaine jour) const { return programmation[jour]; }

    private:
        //Identifiant de zone
        byte idZone;
        String nom;

        float tempConsigne;
        float tempAmbiance;
        float tempDepart;
        Mode mode;
        float tempConfort;
        float tempReduit;
        float tempHorsGel;
        uint8_t programmation[7][6]; // 7 jours, 6 octets par jour
};

#endif //__ZONE_H_