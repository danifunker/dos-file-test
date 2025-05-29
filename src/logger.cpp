#include "logger.h"
#include <fstream.h>
#include <iostream.h>
#include <time.h>
#include <string.h>

// Changed to use longs instead of doubles
void Logger::logTransferDetails(const char* source, const char* destination, 
                               long fileSize, long maxSpeed, long minSpeed, 
                               long avgSpeed, long duration) {
    // Simpler log file handling to avoid errors
    char logPath[256] = "TRANSFER.LOG";  // Use current directory
    
    ofstream logFile(logPath, ios::app);
    
    if (!logFile) {
        cerr << "Error opening log file!" << endl;
        return;
    }

    // Get current time
    time_t now = time(NULL);
    struct tm* localTime = localtime(&now);
    char timeBuffer[80];
    strftime(timeBuffer, 80, "%Y-%m-%d %H:%M:%S", localTime);

    // Convert speeds to KB/sec using integer division
    long maxSpeedKB = maxSpeed / 1024;
    long minSpeedKB = minSpeed / 1024;
    long avgSpeedKB = avgSpeed / 1024;

    logFile << "Transfer Log" << endl;
    logFile << "Date and Time: " << timeBuffer << endl;
    logFile << "Source: " << source << endl;
    logFile << "Destination: " << destination << endl;
    logFile << "Size: " << fileSize << " bytes" << endl;
    logFile << "Max: " << maxSpeedKB << " KB/sec" << endl;
    logFile << "Min: " << minSpeedKB << " KB/sec" << endl;
    logFile << "Avg: " << avgSpeedKB << " KB/sec" << endl;
    logFile << "Time: " << duration << " seconds" << endl;
    logFile << "----------------------------------------" << endl;

    logFile.close();
}