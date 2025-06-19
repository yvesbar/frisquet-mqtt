#ifndef __CONNECT_H_
#define __CONNECT_H_

#include <Arduino.h>

class Connect {
public:
    static Connect& getInstance() {
        static Connect instance;
        return instance;
    }
    byte getId() const { return idConnect; }
    void setId(byte id) { idConnect = id; }
    bool isConnectAssocie() const { return idConnect != 0; }
private:
    Connect() : idConnect(0) {} // Constructeur privé, id par défaut à 0
    Connect(const Connect&) = delete;
    Connect& operator=(const Connect&) = delete;
    byte idConnect;
};

#endif // __CONNECT_H_
