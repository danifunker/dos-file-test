#include "logger.h"
#include <fstream.h>
#include <iostream.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <dir.h>  // For directory handling

// Function to format speed for logging
void formatSpeedForLog(long bytesPerSec, char* buffer) {
    if (bytesPerSec <= 0) {
        strcpy(buffer, "0.00 KB/s");
        return;
    }
    
    // Use MB/s for speeds over 2048 KB/s (2 MB/s)
    if (bytesPerSec >= 2048 * 1024) {
        double mbPerSec = (double)bytesPerSec / (1024.0 * 1024.0);
        sprintf(buffer, "%.2f MB/s", mbPerSec);
    } else {
        double kbPerSec = (double)bytesPerSec / 1024.0;
        sprintf(buffer, "%.2f KB/s", kbPerSec);
    }
}

// Extract the directory path from a full file path
void extractDirectory(const char* filePath, char* dirPath) {
    strcpy(dirPath, filePath);
    
    // Find the last backslash or forward slash
    char* lastSlash = strrchr(dirPath, '\\');
    char* lastFwdSlash = strrchr(dirPath, '/');
    
    // Use the rightmost slash
    char* lastSep = (lastFwdSlash > lastSlash) ? lastFwdSlash : lastSlash;
    
    if (lastSep) {
        // Truncate after the last slash to get directory path
        *(lastSep + 1) = '\0';
    } else {
        // No directory separator found, use current directory
        strcpy(dirPath, ".\\");
    }
}

// Changed to use longs instead of doubles
void Logger::logTransferDetails(const char* source, const char* destination, 
                               long fileSize, long maxSpeed, long minSpeed, 
                               long avgSpeed, long duration) {
    // Extract the destination directory
    char destDir[256];
    extractDirectory(destination, destDir);
    
    // Create log file path in the destination directory
    char logPath[256];
    strcpy(logPath, destDir);
    strcat(logPath, "TRANSFER.LOG");
    
    ofstream logFile(logPath, ios::app);
    
    if (!logFile) {
        cerr << "Error opening log file: " << logPath << endl;
        return;
    }

    // Get current time
    time_t now = time(NULL);
    struct tm* localTime = localtime(&now);
    char timeBuffer[80];
    strftime(timeBuffer, 80, "%Y-%m-%d %H:%M:%S", localTime);

    // Format speeds with proper units and precision
    char maxSpeedStr[20];
    char minSpeedStr[20];
    char avgSpeedStr[20];
    
    formatSpeedForLog(maxSpeed, maxSpeedStr);
    formatSpeedForLog(minSpeed, minSpeedStr);
    formatSpeedForLog(avgSpeed, avgSpeedStr);

    logFile << "Transfer Log" << endl;
    logFile << "Date and Time: " << timeBuffer << endl;
    logFile << "Source: " << source << endl;
    logFile << "Destination: " << destination << endl;
    logFile << "Size: " << fileSize << " bytes" << endl;
    logFile << "Max: " << maxSpeedStr << endl;
    logFile << "Min: " << minSpeedStr << endl;
    logFile << "Avg: " << avgSpeedStr << endl;
    logFile << "Time: " << duration << " seconds" << endl;
    logFile << "----------------------------------------" << endl;

    logFile.close();
}