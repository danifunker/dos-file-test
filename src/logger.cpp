#include "logger.h"
#include <fstream.h>
#include <iostream.h>
#include <time.h>
#include <string.h>
#include <iomanip.h>

void Logger::logTransferDetails(const char* source, const char* destination, 
                               long fileSize, double maxSpeed, double minSpeed, 
                               double avgSpeed, double duration) {
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

    // Convert speeds from bytes/sec to KB/sec (not MB) to avoid large fp numbers
    double maxSpeedKB = maxSpeed / 1024.0;
    double minSpeedKB = minSpeed / 1024.0;
    double avgSpeedKB = avgSpeed / 1024.0;

    logFile << "Transfer Log" << endl;
    logFile << "Date and Time: " << timeBuffer << endl;
    logFile << "Source: " << source << endl;
    logFile << "Destination: " << destination << endl;
    logFile << "Size: " << fileSize << " bytes" << endl;
    logFile << "Max: " << (long)maxSpeedKB << " KB/sec" << endl;
    logFile << "Min: " << (long)minSpeedKB << " KB/sec" << endl;
    logFile << "Avg: " << (long)avgSpeedKB << " KB/sec" << endl;
    logFile << "Time: " << (long)duration << " seconds" << endl;
    logFile << "----------------------------------------" << endl;

    logFile.close();
}