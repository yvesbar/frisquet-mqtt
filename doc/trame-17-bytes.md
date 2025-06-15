Provient de https://github.com/Burnallover/frisquet-mqtt/wiki

Satellite

| Byte | Valeur | Description        | Commentaire             |
|------|--------|--------------------|-------------------------|
| 0 | 0x11 | Nombre de bytes dans le message | 17 |
| 1 | 0x80 | Adresse de destination | Chaudière |
| 2 | 0x20| Adresse de source | Satellite|
| 3 |  | Request Id | Défini lors de la phase d'association |
| 4 | 0x10 | Numéro de message | Incrémenté à chaque nouvel échange |
| 5 | 0x01 | Type de message | demande=0x01, réponse=0x81 |
| 6 | 0x17 | Type de message | constant  |
| 7 | 0xA0 | Type de message | constant  |
| 8    | u8     | Unknown2           | One-byte value          |
| 9-10 | [u8; 2] | Message Static Part| Two-byte array          |
| 11-12| i16    | Temperature        | Two-byte signed integer |
| 13-14| i16    | Consigne            | Two-byte signed integer |
| 15   | u8     | Unknown Mode 1     | One-byte value, 3 bits  |
| 15   | bool   | Hors Gel           | One-bit boolean         |
| 15   | u8     | Unknown Mode 2     | One-byte value, 2 bits  |
| 15   | bool   | Derogation         | One-bit boolean         |
| 15   | bool   | Soleil             | One-bit boolean         |
| 16-17| [u8; 2] | Signature         | Two-byte array          |

Sonde

| Byte | Valeur | Description | Commentaire |
|---|---|---|---|
| 0 | 0x11 | Nombre de bytes dans le message | 17 |
| 1 | 0x80 | Adresse de destination | Chaudière |
| 2 | 0x20| Adresse de source | Sonde |
| 3 |  | Request Id | Défini lors de la phase d'association |
| 4 | 0x10 | Numéro de message | Incrémenté à chaque nouvel échange |
| 5 | 0x01 | Type de message | demande=0x01, réponse=0x81 |
| 6 | 0x17 | Type de message | constant  |
| 7 |  | |  |
| 8 |  | |  | 
| 9 |  | |  | 
| 10 |  | |  | 
| 11 |  | |  | 
| 12 |  | |  | 
| 13 |  | |  | 
| 14 |  | |  | 
| 15 |  | | |
| 16 | 0x00 | temperature négative ou positive | quelle valeur quand négative?  | 
| 17 | 0xBF  | Température | `191` | 

