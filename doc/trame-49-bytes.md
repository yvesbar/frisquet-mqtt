Provient de https://github.com/Burnallover/frisquet-mqtt/wiki

les températures sont encodées en tant qu'entiers signés sur 16 bits, avec une multiplication par 10 pour inclure une précision décimale, et en utilisant le format complément à deux pour les valeurs négatives.

| Byte | Value | Description | Comment |
|---|---|---|---|
| 1 | 0x08 | Adresse de destination | Satellite=0x08 |
| 2 | 0x80 | Adresse de source | Chaudière=0x80 |
| 3 | 0x6E | Request Id | Échange chaudière/Satellite (valeur définie lors de la phase d'association) |
| 4 | 0x10 | Numéro de message | Incrémenté de 4 à chaque nouvel échange. Identique au précédent si c'est une réponse |
| 5 | 0x81 | | demande=0x01, réponse=0x81) |
| 6 | 0x17 | Type de message constant | 0x17=std  |
| 7 | 0xA0 0x2A  | Type de message constant  | Satellite = A0 réponse chaudiere 2A  |
| /!\ 8-9 | 0x00 0x61 | Température extérieure x10 ? a vérifier | 0061 = 9,7 /FFB2 = -7,8 |
| 10-11 | 0x00 0x00 | Inconnu | |
| 12-17 | 23 12 10 12 40 12 | YY MM DD HH MM SS | |
| 18-19 | | | |
| 20-21 | 0x00 0xD6 | Température intérieure x10 | 0xD6 = 21,4 (codé sur xx bits ???) |
| 22-23 | 0x00 0xAF | Température consigne x10 | 0xAF = 17,5 |
|    24   |   00    |                     |           |
|    25   |   05    |                     |           |
|    26   |   01    |                     |           |
|    27   |   3A    |                     |           |
|    28   |   01    |                     |           |
|    29   |   54    |                     |           |
|    30   |   04    |                     |           |
|    31   |   F6    |                     |           |
|    32   |   00    |                     |           |
|    33   |   00    |                     |           |
|    34   |   00    |                     |           |
|    35   |   00    |                     |           |
|    36   |   00    |                     |           |
|    37   |   00    |                     |           |
|    38   |   00    |                     |           |
|    39   |   00    |                     |           |
|    40   |   04    |                     |           |
|    41   |   F6    |                     |           |
|    42   |   00    |                     |           |
|    43   |   00    |                     |           |
|    44   |   00    |                     |           |
|    45   |   00    |                     |           |
|    46   |   00    |                     |           |
|    47   |   00    |                     |           |
|    48   |   00    |                     |           |
