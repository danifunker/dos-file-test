#ifndef UTILS_H
#define UTILS_H

#include <string.h>
#include <time.h>

// Utility function to get the current time as a formatted string
void getCurrentTime(char* buffer, int bufferSize) {
    time_t now = time(0);
    struct tm* localTime = localtime(&now);
    strftime(buffer, bufferSize, "%Y-%m-%d %X", localTime);
}

// Utility function to calculate the speed in bytes per second
double calculateSpeed(long bytesTransferred, double duration) {
    return (duration > 0) ? (bytesTransferred / duration) : 0;
}

#endif // UTILS_H