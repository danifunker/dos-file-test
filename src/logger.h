#ifndef LOGGER_H
#define LOGGER_H

class Logger {
public:
    void logTransferDetails(const char* source, const char* destination, 
                            long fileSize, double maxSpeed, double minSpeed, 
                            double avgSpeed, double duration);
};

#endif // LOGGER_H