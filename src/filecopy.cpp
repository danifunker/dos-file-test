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

    // Use a smaller buffer size for better DOS compatibility
    const int BUFFER_SIZE = 4096;  // Reduced from 8KB to 4KB
    char buffer[BUFFER_SIZE];
    long bytesRead;
    time_t startTime = time(NULL);
    long totalBytesCopied = 0;
    double maxSpeed = 0.0, minSpeed = 0.0, totalSpeed = 0.0;
    int speedCount = 0;
    
    // Update progress less frequently for large files
    long updateInterval = fileSize / 100;  // Update roughly 100 times total
    if (updateInterval < 4096) updateInterval = 4096;  // Minimum interval
    if (updateInterval > 262144) updateInterval = 262144;  // Maximum 256KB interval
    
    long nextUpdateAt = updateInterval;
    
    Progress progress;
    progress.showProgressBar(0, fileSize, 0.0);

    // Process file in manageable chunks
    while (!sourceFile.eof() && totalBytesCopied < fileSize) {
        sourceFile.read(buffer, BUFFER_SIZE);
        bytesRead = sourceFile.gcount();
        
        if (bytesRead <= 0) break;
        
        destFile.write(buffer, bytesRead);
        totalBytesCopied += bytesRead;

        // Update progress bar less frequently to reduce overhead
        if (totalBytesCopied >= nextUpdateAt || totalBytesCopied == fileSize) {
            time_t currentTime = time(NULL);
            double duration = difftime(currentTime, startTime);
            double currentSpeed = (duration > 0) ? ((double)totalBytesCopied / duration) : 0.0;

            if (speedCount == 0) {
                maxSpeed = minSpeed = currentSpeed;
            } else {
                if (currentSpeed > maxSpeed) maxSpeed = currentSpeed;
                if (currentSpeed < minSpeed && currentSpeed > 0) minSpeed = currentSpeed;
            }

            totalSpeed += currentSpeed;
            speedCount++;

            // Simple progress without complex formatting
            int percent = (int)((totalBytesCopied * 100) / fileSize);
            cout << "Progress: " << percent << "% - "
                 << (currentSpeed / 1024.0) << " KB/sec      \r";
            cout.flush();
            
            nextUpdateAt = totalBytesCopied + updateInterval;
        }
        
        // Add a small delay to prevent hogging CPU and allow DOS to handle I/O
        delay(1);  // 1ms delay between chunks
    }

    // Final update
    time_t endTime = time(NULL);
    double totalDuration = difftime(endTime, startTime);
    double averageSpeed = (totalDuration > 0) ? ((double)totalBytesCopied / totalDuration) : 0.0;

    cout << "\nCopy complete: " << fileSize << " bytes in " 
         << totalDuration << " seconds" << endl;
    cout << "Average speed: " << (averageSpeed / 1024.0 / 1024.0) << " MB/sec" << endl;

    Logger logger;
    logger.logTransferDetails(sourcePath, destPath, fileSize, maxSpeed, minSpeed, averageSpeed, totalDuration);

    sourceFile.close();
    destFile.close();
}