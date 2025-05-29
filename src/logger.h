#ifndef LOGGER_H
#define LOGGER_H

class Logger {
public:
    // Changed to use longs instead of doubles
    void logTransferDetails(const char* source, const char* destination, 
                            long fileSize, long maxSpeed, long minSpeed, 
                            long avgSpeed, long duration);
};

#endif // LOGGER_H