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
    
    // Larger buffer for better performance
    const int BUFFER_SIZE = 8192;  // 8KB buffer for faster copying
    char buffer[BUFFER_SIZE];
    long bytesRead;
    time_t startTime = time(NULL);
    long totalBytesCopied = 0;
    
    // Track min/max speeds using integers only
    long maxBytesPerSec = 0;
    long minBytesPerSec = 0;
    int speedCount = 0;
    
    // Time-based progress updates (every 0.5 seconds)
    time_t lastUpdateTime = startTime;
    
    cout << "Starting copy operation..." << endl;
    
    // Initial progress display
    Progress progress;
    progress.showProgressBar(0, fileSize, 0);
    
    // Process file in manageable chunks
    while (sourceFile && totalBytesCopied < fileSize) {
        sourceFile.read(buffer, BUFFER_SIZE);
        bytesRead = sourceFile.gcount();
        
        if (bytesRead <= 0) break;
        
        destFile.write(buffer, bytesRead);
        if (!destFile) {
            cerr << "Error writing to destination file" << endl;
            sourceFile.close();
            destFile.close();
            return;
        }
        
        totalBytesCopied += bytesRead;

        // Check if it's time to update progress (every 0.5 seconds)
        time_t currentTime = time(NULL);
        if (difftime(currentTime, lastUpdateTime) >= 0.5) {
            long elapsedSecs = (long)difftime(currentTime, startTime);
            
            // Avoid division by zero
            long currentBytesPerSec = 0;
            if (elapsedSecs > 0) {
                currentBytesPerSec = totalBytesCopied / elapsedSecs;
            }

            // Update min/max speed - avoid floating point
            if (speedCount == 0) {
                maxBytesPerSec = minBytesPerSec = currentBytesPerSec;
            } else {
                if (currentBytesPerSec > maxBytesPerSec) maxBytesPerSec = currentBytesPerSec;
                if (currentBytesPerSec < minBytesPerSec && currentBytesPerSec > 0) 
                    minBytesPerSec = currentBytesPerSec;
            }

            speedCount++;
            
            // Update progress display - pass speed as a long to avoid conversion
            progress.showProgressBar(totalBytesCopied, fileSize, (double)currentBytesPerSec);
            
            // Remember when we last updated
            lastUpdateTime = currentTime;
        }
    }

    // Close files as soon as copy is complete
    sourceFile.close();
    destFile.close();

    // Final update
    time_t endTime = time(NULL);
    long totalDuration = (long)difftime(endTime, startTime);
    
    // Avoid division by zero
    long avgBytesPerSec = 0;
    if (totalDuration > 0) {
        avgBytesPerSec = totalBytesCopied / totalDuration;
    }

    // Show final progress - avoid showing progress bar at the end
    cout << "\nCopy complete: " << fileSize << " bytes in " 
         << totalDuration << " seconds" << endl;
         
    long avgKBPerSec = avgBytesPerSec / 1024;
    cout << "Average speed: " << avgKBPerSec << " KB/sec" << endl;

    // Only log if we copied the entire file
    if (totalBytesCopied == fileSize) {
        try {
            // Convert to double only at the last moment for the log file
            double maxSpeedD = (double)maxBytesPerSec;
            double minSpeedD = (double)minBytesPerSec;
            double avgSpeedD = (double)avgBytesPerSec;
            double durationD = (double)totalDuration;
    
            Logger logger;
            logger.logTransferDetails(sourcePath, destPath, fileSize, maxSpeedD, minSpeedD, 
                                     avgSpeedD, durationD);
        }
        catch (...) {
            cerr << "Warning: Could not write log file" << endl;
        }
    }
}