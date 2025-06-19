Provient de https://github.com/Burnallover/frisquet-mqtt/wiki

# Information sur une ZONE



Format de la trame 63 octets Satellite (08h)  vers Chaudière (80h) à destination de Connect (7Eh):

| Dest| Src| Id |inc |Dest |Type |4 u8          |4 u8           |nb data        |Confort  |Réduit    |H.Gel  |Mode |00  |Dimanche  |Lundi  |Mardi |Mercredi         |Jeudi               | Vendredi          |Samedi|
|------|----|---|----|------|------|-----------|-------------|--------------|---------|----------|-------|--------|----|------------|------|---|----|----|----|---|
| 80  | 08  |1A |F8 | 7E  | 17    | A1540015 | A1540018  | 30 (48 datas) | 9B (20.5) | 7D (17.5) | 64 (15) | 0525 | 00 |0700E0FFFFFF |0300060080FF |0300060080FF |0300060080FF |0300060080FF |0300060080FF |0700E0FFFFFF |
| 7E  | 80  |1A |F8 | 08 (09 ou 0A)  | 17    | A1540015 | A1540018  | 30 (48 datas) | 9B (20.5) | 7D (17.5) | 64 (15) | 0525 | 00 |0700E0FFFFFF |0300060080FF |0300060080FF |0300060080FF |0300060080FF |0300060080FF |0700E0FFFFFF |

Existe aussi sous le format
(trame[0] == 0x7e && trame[1] == 0x80 && trame[4] == Zone::ZONE1_ID && trame[5] == 0x17)


Codage de la programmation hebdomadaire :

exemple Lundi : 6 octets par jour = 48bits => 0.5heure/bit  (ex: Confort 8h30-9h30 + 19h30-1h):

03 00 06 00 80 FF => Lecture de chaque mot en **LSB first** : 
|11000000 |00000000 |01100000 |00000000 |00000001 |11111111|
|-----------|------------|-----------|------------|-----------|----------|
|1h             |                 |8h30/9h30|               |       19h30|                |

Trames de connect vers Chaudiere changement température

| Bytes|  valeur  |     Description     | Commentaire|
|-----------|------------|-----------|------------|
|    00   |   80    |Adresse de destination|           |
|    01   |   7E    |Adresse source|           |
|    02   |   4B    |Request Id|           |
|    03   |   D8    |Numéro de message|           |
|    04   |   08    |Destinataire final|           |
|    05   |   17    |Types ?|           |
|    06 - 09  |   A1540018    |     ?                |           |
|    10 - 13 |   A1540018    |      ?               |           |
|    14   |   30    |      nb de bytes restant      |           |
15 | 8C | Température confort  |  
16 | 78 | Température réduit |  
17 | 46 | Température Hors gel  |  
18 -19| 05 25|  Mode de chauffage | Confort ? 
20 | 00 |   |  
21 - 26 | 0080FFFFFF3F | programmation dimanche  |   
27 - 32 | 0080FFFFFF3F | programmation lundi  |  
33 - 38 | 0080FFFFFF3F | programmation mardi  |   
39 - 44 | 0080FFFFFF3F | programmation mercredi  |   
45 - 50 | 0080FFFFFF3F | programmation jeudi  |  
51 - 56 | 0080FFFFFF3F | programmation vendredi  |  
57 - 62 | 0080FFFFFF3F | programmation samedi  |

# Informations sur les températures

- Les premiers octets de la trame doivent correspondre à :
  - `trame[0] == 0x7e` //Connect
  - `trame[1] == 0x80` //Chaudière
  - `trame[3] == Numéro de trame`
  - `trame[4] == 0x81` //Réponse de la chaudière (80 +1)
  - `trame[5] == 0x03`

C'est une trame de réponse à une demande du frisquet connect sur l'état des températures : Trame 79 E0 :
`0x80, 0x7e, 0x21, 0xE0, 0x01, 0x03, 0x79, 0xE0, 0x00, 0x1C`


## La trame

| Champ                        | Index (début) | Taille (octets) | Description                                 | Conversion                |
|------------------------------|---------------|-----------------|---------------------------------------------|---------------------------|
| Température ECS              | 7             | 2               | Eau Chaude Sanitaire (dixième de °C)        | (trame[7] << 8) + trame[8] / 10.0 |
| Température CDC              | 9             | 2               | Corps de chauffe (dixième de °C)            | (trame[9] << 8) + trame[10] / 10.0 |
| Zone 1 tempDépart            | 11            | 2               | Température de départ zone 1                | (trame[11] << 8) + trame[12] / 10.0 |
| Zone 2 tempDépart            | 13            | 2               | Température de départ zone 2                | (trame[13] << 8) + trame[14] / 10.0 |
| Zone 3 tempDépart            | 15            | 2               | Température de départ zone 3                | (trame[15] << 8) + trame[16] / 10.0 |
| Zone 1 tempAmbiante          | 43            | 2               | Température ambiante zone 1                 | (trame[43] << 8) + trame[44] / 10.0 |
| Zone 2 tempAmbiante          | 45            | 2               | Température ambiante zone 2                 | (trame[45] << 8) + trame[46] / 10.0 |
| Zone 3 tempAmbiante          | 47            | 2               | Température ambiante zone 3                 | (trame[47] << 8) + trame[48] / 10.0 |
| Zone 1 consigne              | 55            | 2               | Température de consigne zone 1              | (trame[55] << 8) + trame[56] / 10.0 |
| Zone 2 consigne              | 57            | 2               | Température de consigne zone 2              | (trame[57] << 8) + trame[58] / 10.0 |
| Zone 3 consigne              | 59            | 2               | Température de consigne zone 3              | (trame[59] << 8) + trame[60] / 10.0 |
| Température extérieure       | 61            | 2               | Température extérieure                       | (trame[61] << 8) + trame[62] / 10.0 |

**Remarques :**
- Les index sont 0-based (le premier octet de la trame est à l'index 0).
- Les valeurs sont généralement exprimées en dixièmes de degré Celsius (ex : 265 → 26,5 °C).
- Certaines valeurs peuvent être incohérentes (ex : 127,0 °C) si la zone n'est pas active ou non câblée.

### Exemple de trame décodée

Exemple (63 octets) :

```
7E 80 4B 50 81 03 38 02 88 01 3A 01 09 04 F6 04 F6 01 45 00 00 00 00 00 00 00 00 20 00 00 11 00 00 00 00 00 00 00 00 00 00 00 00 00 C4 04 F6 04 F6 01 00 00 C6 00 C6 00 C3 00 00 00 00 05 0A
```

