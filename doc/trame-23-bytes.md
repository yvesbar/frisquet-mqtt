Provient de https://github.com/Burnallover/frisquet-mqtt/wiki

| Byte | Valeur | Description | Commentaire |
|---|---|---|---|
| 0 | 0x17 |Nombre de bytes dans le message | 23|
| 1 | 0x80 | Adresse de destination | Chaudière |
| 2 | 0x08 | Adresse de source | Satellite |
| 3 | 0x6E | Request Id | échange chaudière satellite définie lors de la phase d'association |
| 4 | 0x10 | Numéro de message | Incrémenté à chaque nouvel échange. Identique au précédent si c'est une réponse |
| 5 | 0x01 | Type de message | demande=0x01, réponse=0x81 |
| 6 | 0x17 | Type de message constant | 0x17=std |
| 7 | 0x03 | Type de message |  |
| 8 |  | |  | 
| 9 |  | |  | 
| 10 |  | |  | 
| 11 |  | |  | 
| 12 |  | |  | 
| 13 |  | |  | 
| 14 |  | |  | 
| 15 |  | |  |
| 16-17 | 0x00 0xD6 | Température intérieure x10 | le premier byte doit servir à déterminer la polarité (+ ou -) |
| 18-19 | 0x00 0xAF | Température consigne x10 | le premier byte doit servir à déterminer la polarité (+ ou -) |
| 20 | 0x00 | Inconnu | |
| 21 | 0x20 | Mode | 0x20= mode auto, bit 0=IN, bit 1 =OUT,bit 2=?, bit 3= ?, bit 4= Hors Gel, bit 5= Auto |
| 22-23 | 0x00 0x00 | Inconnu | |