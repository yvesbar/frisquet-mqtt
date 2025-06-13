#ifndef __CHAUDIERE_H_
#define __CHAUDIERE_H_

#include <Arduino.h>
#include <map>
#include "Zone.h"
#include "config.h"
#include "../debug.h"

/*
    Classe représentant la chaudière frisquet
*/ 
class Chaudiere {
    public:
        static Chaudiere& getInstance(); // Méthode pour obtenir l'instance unique
        
        Zone* getZoneById(byte id);        // Récupère une zone par son ID
        Zone* addZone(byte id); // Ajoute une zone au tableau
        
        /* températur extérieure */
        void setTempExterieure(float value);
        float getTempExterieure() const { return tempExterieure; };   // Getter pour tempExterieure
        
        void setTempCorpsDeChauffe(float value);
        float getTempCorpsDeChauffe() const { return tempCorpsDeChauffe; }; // Getter pour tempCorpsDeChauffe

        void setTempECS(float value);
        float getTempECS() const { return tempECS; } // Getter pour tempECS
        
        //TODO ajouter l'identifiant réseau

        static constexpr uint8_t idConnect = 0x7E;
        bool isEcsActive() const { return ECS_ACTIF; }

    private:
        Chaudiere(); // Constructeur privé pour le singleton
        Chaudiere(const Chaudiere&) = delete; // Suppression du constructeur de copie
        Chaudiere& operator=(const Chaudiere&) = delete; // Suppression de l'opérateur d'affectation

        std::map<byte, Zone*> zones;       // Tableau associatif des zones
        float tempExterieure;
        float tempCorpsDeChauffe;
        float tempECS;
        
        //consoGazChauffage
        //consoGazECS
        //

};

#endif  //__CHAUDIERE_H_