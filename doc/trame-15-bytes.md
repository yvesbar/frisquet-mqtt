Provient de https://github.com/Burnallover/frisquet-mqtt/wiki

| Byte | Valeur | Description | Commentaire |
|---|---|---|---|
| 0 | 0x0F | Nombre de bytes dans le message | 15 |
| 1 | 0x20| Adresse de destination | Sonde |
| 2 | 0x80 | Adresse de source | Chaudiere |
| 3 |  | Request Id | Défini lors de la phase d'association |
| 4 | 0x10 | Numéro de message | accusé de reception = au numéro incrementiel reçu |
| 5 | 0x01 | Type de message | demande=0x01, réponse=0x81 |
| 6 | 0x17 | Type de message constant |  |
| 7 | 0xA0 0x2A  | Type de message constant  | Satellite = A0 réponse chaudiere 2A  |
| 8 |  | Année | `23` | 
| 9 |  | Mois | `03` | 
| 10 |  | Jour | `29` | 
| 11 |  | Heure | `16` | 
| 12 |  | Minute | `29` | 
| 13 |  | Seconde | `48` | 
| 14 |  | Mode de chauffe | 20= réduit, 28= confort | 
| 15 |  | numéro séquentiel quotidien | change tous les jours a minuit|