Provient de https://github.com/Burnallover/frisquet-mqtt/wiki

| Payload Type                    | Fields                                                                                                                          | Bytes |
|----------------------------------|----------------------------------------------------------------------------------------------------------------------------------|-------|
| ChaudiereAssociationBroadcast   | `unknown: u8`, `network_id: [u8; 4]`                                                                                            | 11    |
| ChaudiereSondeResponseMessage   | `unknown_start: u8`, `year: u8`, `month: u8`, `day: u8`, `hour: u8`, `minute: u8`, `second: u8`, `data: Vec<u8>`              | 15    |
| ChaudiereSetTemperatureMessageResponse | `unknown_start: [u8; 2]`, `temperature_exterieure: i16`, `unknown: u8`, `year: u8`, `month: u8`, `day: u8`, `hour: u8`, `minute: u8`, `second: u8`, `unknown_1: [u8; 3]`, `temperature: i16`, `consigne: i16`, `unknown_2: [u8; 2]`, `signature: [u8; 3]`, `static_part_2: [u8; 20]` | 49    |
| ChaudiereToSatelliteUnknownMessageResponse | `data: Vec<u8>`                                                                                                                 | 55    |
| ChaudiereUnknownMessage          | `data: Vec<u8>`                                                                                                                 | _     |
