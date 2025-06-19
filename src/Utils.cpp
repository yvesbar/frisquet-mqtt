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
    int value = (highByte << 8) | lowByte;
    return value / 10.0;
}
