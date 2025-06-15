Provient de https://github.com/Burnallover/frisquet-mqtt/wiki
| Payload Type                    | Fields                                     | Bytes |
|----------------------------------|--------------------------------------------|-------|
| SatelliteInitMessage             | `static_part: [u8; 7]`, `message_part: [u8; 3]`                     | 17    |
| SatelliteInitEmptyMessage        | `data: Vec<u8>`                           | 8     |
| SatelliteAssocationAnnounceMessage | `unknown: u8`, `version: [u8; 3]`                                 | 10    |
| SatelliteSetTemperatureMessage   | Various fields representing temperature-related data | 23    |
| SatelliteUnknowMessage           | `data: Vec<u8>`                           | _     |
