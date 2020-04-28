#include "Util.h"

bool isBigEndian() {
    int a = 1;
    return !((char*)&a)[0];
}

int convertToInt(char* buffer, int len) {
    // Convert some chars into a float
    int a = 0;
    if (!isBigEndian()) {
        for (int i = 0; i < len; i++) {
            ((char*)&a)[i] = buffer[i];
        }
    } else {
        for (int i = 0; i < len; i++) {
            ((char*)&a)[3 - i] = buffer[i];
        }
    }
    return a;
}