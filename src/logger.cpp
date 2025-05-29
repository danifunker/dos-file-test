#include "logger.h"
#include <fstream.h>
#include <iostream.h>
#include <time.h>
#include <string.h>
#include <iomanip.h>

void Logger::logTransferDetails(const char* source, const char* destination, 
                               long fileSize, double maxSpeed, double minSpeed, 
                               double avgSpeed, double duration) {
    // Create log file path
    char logPath[256];
    strcpy(logPath, destination);
    
    // Find the last backslash or forward slash
    char* lastSlash = strrchr(logPath, '\\');
    if (!lastSlash) {
        lastSlash = strrchr(logPath, '/');
    }
    
    if (lastSlash) {
        *(lastSlash + 1) = '\0'; // Truncate after the slash
    } else {
        logPath[0] = '\0'; // Empty string if no slash found
    }
    
    // Append log filename
    strcat(logPath, "transfer.log");
    
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

    // Convert speeds from bytes/sec to MB/sec
    double maxSpeedMB = maxSpeed / (1024.0 * 1024.0);
    double minSpeedMB = minSpeed / (1024.0 * 1024.0);
    double avgSpeedMB = avgSpeed / (1024.0 * 1024.0);

    logFile << "Transfer Log" << endl;
    logFile << "Date and Time: " << timeBuffer << endl;
    logFile << "File Source: " << source << endl;
    logFile << "File Destination: " << destination << endl;
    logFile << "File Size: " << fileSize << " bytes" << endl;
    logFile << "Maximum Speed: " << setprecision(2) << setiosflags(ios::fixed) 
            << maxSpeedMB << " MB/sec" << endl;
    logFile << "Minimum Speed: " << setprecision(2) << setiosflags(ios::fixed) 
            << minSpeedMB << " MB/sec" << endl;
    logFile << "Average Speed: " << setprecision(2) << setiosflags(ios::fixed) 
            << avgSpeedMB << " MB/sec" << endl;
    logFile << "Total Duration: " << setprecision(2) << setiosflags(ios::fixed) 
            << duration << " seconds" << endl;
    logFile << "----------------------------------------" << endl;

    logFile.close();
}