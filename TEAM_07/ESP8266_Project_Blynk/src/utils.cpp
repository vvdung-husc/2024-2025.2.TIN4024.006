#include "utils.h"
#include <stdarg.h>

bool IsReady(unsigned long &uTimer, uint32_t millisecond) {
    if (millis() - uTimer < millisecond) return false;
    uTimer = millis();
    return true;
}

String StringFormat(const char* fmt, ...) {
    va_list vaArgs;
    va_start(vaArgs, fmt);
    va_list vaArgsCopy;
    va_copy(vaArgsCopy, vaArgs);
    const int len = vsnprintf(NULL, 0, fmt, vaArgsCopy);
    va_end(vaArgsCopy);

    int size = len + 1;
    char* buff = (char*)malloc(size);
    vsnprintf(buff, size, fmt, vaArgs);
    va_end(vaArgs);

    String s = buff;
    free(buff);
    return s;
}
