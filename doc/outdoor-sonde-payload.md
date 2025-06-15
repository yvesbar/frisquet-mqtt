Provient de https://github.com/Burnallover/frisquet-mqtt/wiki
| Payload Type                    | Fields                                     | Bytes |
|----------------------------------|--------------------------------------------|-------|
| SondeTemperatureMessage          | `data: [u8; 9]`, `temperature: i16`        | 17    |
| SondeAssociationAnnounceMessage  | `data: Vec<u8>`                           | 6     |
| SondeInitMessage                  | `data: Vec<u8>`                           | 8     |
| SondeUnknownMessage               | `data: Vec<u8>`                           | _     |
