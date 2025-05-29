#include <iostream.h>
#include <fstream.h>
#include <string.h>
#include <time.h>
#include <dos.h>
#include <dir.h>
#include "filecopy.h"
#include "progress.h"
#include "logger.h"

// Normalize a file path to ensure it's valid for DOS
void normalizePath(char* path) {
    // Convert forward slashes to backslashes
    for (int i = 0; path[i] != '\0'; i++) {
        if (path[i] == '/') {
            path[i] = '\\';
        }
    }
}

void FileCopy::copyFile(const char* sourcePath, const char* destPath) {
    char normalizedSource[256];
    char normalizedDest[256];
    
    // Make copies so we can safely modify them
    strcpy(normalizedSource, sourcePath);
    strcpy(normalizedDest, destPath);
    
    // Convert slashes for DOS compatibility
    normalizePath(normalizedSource);
    normalizePath(normalizedDest);
    
    // Open files with binary mode for proper handling
    ifstream sourceFile(normalizedSource, ios::binary);
    ofstream destFile(normalizedDest, ios::binary);

    if (!sourceFile) {
        cerr << "Error opening source file: " << normalizedSource << endl;
        return;
    }
    if (!destFile) {
        cerr << "Error opening destination file: " << normalizedDest << endl;
        return;
    }

    // Get file size
    sourceFile.seekg(0, ios::end);
    long fileSize = sourceFile.tellg();
    sourceFile.seekg(0, ios::beg);

    // Use a modest buffer size - 8KB is good for DOS
    const int BUFFER_SIZE = 8192;
    char buffer[BUFFER_SIZE];
    long bytesRead;
    time_t startTime = time(NULL);
    long totalBytesCopied = 0;
    double maxSpeed = 0.0, minSpeed = 0.0, totalSpeed = 0.0;
    int speedCount = 0;
    time_t lastUpdateTime = startTime;
    time_t currentTime;

    Progress progress;
    progress.showProgressBar(0, fileSize, 0.0);

    // Process file in manageable chunks
    while (sourceFile && !sourceFile.eof() && totalBytesCopied < fileSize) {
        sourceFile.read(buffer, BUFFER_SIZE);
        bytesRead = sourceFile.gcount();
        
        if (bytesRead <= 0) break;
        
        destFile.write(buffer, bytesRead);
        totalBytesCopied += bytesRead;

        // Update speed calculation less frequently to avoid console flicker
        currentTime = time(NULL);
        if (difftime(currentTime, lastUpdateTime) >= 1.0) {
            double duration = difftime(currentTime, startTime);
            double currentSpeed = (duration > 0) ? (totalBytesCopied / duration) : 0.0;

            if (speedCount == 0) {
                maxSpeed = minSpeed = currentSpeed;
            } else {
                if (currentSpeed > maxSpeed) maxSpeed = currentSpeed;
                if (currentSpeed < minSpeed && currentSpeed > 0) minSpeed = currentSpeed;
            }

            totalSpeed += currentSpeed;
            speedCount++;

            progress.showProgressBar(totalBytesCopied, fileSize, currentSpeed);
            lastUpdateTime = currentTime;
        }
    }

    // Final update
    time_t endTime = time(NULL);
    double totalDuration = difftime(endTime, startTime);
    double averageSpeed = (totalDuration > 0) ? (totalBytesCopied / totalDuration) : 0.0;

    // Show 100% completion
    progress.showProgressBar(fileSize, fileSize, averageSpeed);

    Logger logger;
    logger.logTransferDetails(sourcePath, destPath, fileSize, maxSpeed, minSpeed, averageSpeed, totalDuration);

    sourceFile.close();
    destFile.close();
}