#include "Utils.h"

String Utils::byteArrayToHexString(byte* byteArray, int length) {
    String result = "";
    for (int i = 0; i < length; i++) {
        char hex[3];
        sprintf(hex, "%02X", byteArray[i]);
        result += hex;
    }
    return result;
}

float Utils::bytesToTemperature(byte highByte, byte lowByte) {
    // Combiner les octets puis interpréter en signé (complément à 2) pour gérer les températures négatives
    uint16_t rawValue = (highByte << 8) | lowByte;
    int16_t signedValue = static_cast<int16_t>(rawValue);
    return signedValue / 10.0;
}
