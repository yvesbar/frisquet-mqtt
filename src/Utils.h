#ifndef __UTILS_H_
#define __UTILS_H_

#include <Arduino.h>

class Utils {
public:
    static String byteArrayToHexString(byte* byteArray, int length);
};

#endif // __UTILS_H_
