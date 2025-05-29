/* 
 * FileCopy Utility
 * Copyright (C) 2025 Daniel Funke
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 * This file contains code inspired by the FreeDOS xcopy utility
 * Source: https://github.com/FDOS/xcopy/tree/master/source
 */

#include <iostream.h>
#include <fstream.h>
#include <string.h>
#include <time.h>
#include <dos.h>
#include <dir.h>
#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include "filecopy.h"
#include "progress.h"
#include "logger.h"

// Debug flag
const int DEBUG_MODE = 1;

// Simple stack tracking for debugging
int stackDepth = 0;
char stackTrace[10][80]; // Store up to 10 function calls

void enterFunction(const char* funcName) {
    if (stackDepth < 10) {
        sprintf(stackTrace[stackDepth], "-> %s", funcName);
        stackDepth++;
    }
    
    if (DEBUG_MODE != 0) {
        cout << "[DEBUG] Entering: " << funcName << endl;
        cout << "[DEBUG] Stack depth: " << stackDepth << endl;
    }
}

void exitFunction(const char* funcName) {
    if (stackDepth > 0) {
        stackDepth--;
    }
    
    if (DEBUG_MODE != 0) {
        cout << "[DEBUG] Exiting: " << funcName << endl;
    }
}

void printStack() {
    cout << "Stack trace:" << endl;
    for (int i = 0; i < stackDepth; i++) {
        cout << stackTrace[i] << endl;
    }
}

// Debug print function
void debugPrint(const char* message) {
    if (DEBUG_MODE != 0) {
        cout << "[DEBUG] " << message << endl;
    }
}

// Normalize a file path to ensure it's valid for DOS
void normalizePath(char* path) {
    enterFunction("normalizePath");
    
    // Convert forward slashes to backslashes
    for (int i = 0; path[i] != '\0'; i++) {
        if (path[i] == '/') {
            path[i] = '\\';
        }
    }
    
    exitFunction("normalizePath");
}

// This implementation is based on FreeDOS xcopy's direct file copy mechanism
// which uses low-level file I/O functions for better stability
void FileCopy::copyFile(const char* sourcePath, const char* destPath) {
    enterFunction("copyFile");
    debugPrint("Copy operation started");
    
    // Use static buffers to reduce stack usage
    static char normalizedSource[256];
    static char normalizedDest[256];
    
    // Make copies so we can safely modify them
    strcpy(normalizedSource, sourcePath);
    strcpy(normalizedDest, destPath);
    
    // Convert slashes for DOS compatibility
    normalizePath(normalizedSource);
    normalizePath(normalizedDest);
    
    debugPrint("Opening source file");
    
    // Open source file using low-level file I/O
    int sourceHandle = open(normalizedSource, O_RDONLY | O_BINARY);
    if (sourceHandle < 0) {
        cerr << "Error opening source file: " << normalizedSource << endl;
        exitFunction("copyFile");
        return;
    }
    
    debugPrint("Getting file size");
    // Get file size using filelength() which is more reliable in DOS
    long fileSize = filelength(sourceHandle);
    
    static char sizeMsg[80];
    sprintf(sizeMsg, "File size: %ld bytes", fileSize);
    debugPrint(sizeMsg);
    
    debugPrint("Opening destination file");
    // Open destination file
    int destHandle = open(normalizedDest, O_WRONLY | O_CREAT | O_TRUNC | O_BINARY, S_IREAD | S_IWRITE);
    if (destHandle < 0) {
        cerr << "Error opening destination file: " << normalizedDest << endl;
        close(sourceHandle);
        exitFunction("copyFile");
        return;
    }
    
    // Buffer size - 16KB is a good balance for DOS
    const int BUFFER_SIZE = 16384;
    debugPrint("Allocating buffer");
    static char buffer[BUFFER_SIZE]; // Static to avoid stack issues
    
    time_t startTime = time(NULL);
    long totalBytesCopied = 0;
    
    // Speed tracking variables
    long maxBytesPerSec = 0;
    long minBytesPerSec = 2147483647L;
    int speedCount = 0;
    
    debugPrint("Setting up progress tracking");
    time_t lastUpdateTime = startTime;
    
    cout << "Starting copy operation..." << endl;
    
    // Initial progress display
    Progress progress;
    debugPrint("Showing initial progress");
    progress.showProgressBar(0, fileSize, 0);
    
    debugPrint("Starting copy loop");
    
    try {
        int bytesRead;
        long loopCount = 0;
        
        // Copy in chunks
        while ((bytesRead = read(sourceHandle, buffer, BUFFER_SIZE)) > 0) {
            loopCount++;
            
            // Debug output for every 100th iteration
            if (loopCount % 100 == 0) {
                static char loopMsg[80];
                sprintf(loopMsg, "Copy loop iteration: %ld, Bytes copied: %ld", loopCount, totalBytesCopied);
                debugPrint(loopMsg);
            }
            
            // Write the chunk to destination
            int bytesWritten = write(destHandle, buffer, bytesRead);
            if (bytesWritten != bytesRead) {
                cerr << "Error writing to destination file" << endl;
                debugPrint("Write error, closing files");
                close(sourceHandle);
                close(destHandle);
                exitFunction("copyFile");
                return;
            }
            
            totalBytesCopied += bytesRead;
            
            // Update progress display every second
            time_t currentTime = time(NULL);
            if (difftime(currentTime, lastUpdateTime) >= 1.0) {
                debugPrint("Updating progress display");
                long elapsedSecs = (long)difftime(currentTime, startTime);
                
                // Calculate speed (bytes per second)
                long currentBytesPerSec = 0;
                if (elapsedSecs > 0) {
                    currentBytesPerSec = totalBytesCopied / elapsedSecs;
                }
                
                // Update min/max speed - integer only
                if (speedCount == 0) {
                    maxBytesPerSec = minBytesPerSec = currentBytesPerSec;
                } else {
                    if (currentBytesPerSec > maxBytesPerSec) maxBytesPerSec = currentBytesPerSec;
                    if (currentBytesPerSec < minBytesPerSec && currentBytesPerSec > 0) 
                        minBytesPerSec = currentBytesPerSec;
                }
                
                speedCount++;
                
                // Update progress display
                progress.showProgressBar(totalBytesCopied, fileSize, currentBytesPerSec);
                
                lastUpdateTime = currentTime;
            }
            
            // Let DOS breathe occasionally
            if (loopCount % 20 == 0) {
                delay(1);
            }
        }
    }
    catch (...) {
        debugPrint("Exception during copy loop");
        printStack();
    }
    
    debugPrint("Copy loop complete, closing files");
    // Close both files
    close(sourceHandle);
    close(destHandle);
    
    debugPrint("Calculating final statistics");
    time_t endTime = time(NULL);
    long totalDuration = (long)difftime(endTime, startTime);
    
    long avgBytesPerSec = 0;
    if (totalDuration > 0) {
        avgBytesPerSec = totalBytesCopied / totalDuration;
    }
    
    debugPrint("Displaying completion message");
    cout << "\nCopy complete: " << fileSize << " bytes in " 
         << totalDuration << " seconds" << endl;
         
    long avgKBPerSec = avgBytesPerSec / 1024;
    cout << "Average speed: " << avgKBPerSec << " KB/sec" << endl;
    
    if (totalBytesCopied == fileSize) {
        debugPrint("Writing log file");
        try {
            Logger logger;
            logger.logTransferDetails(sourcePath, destPath, fileSize, 
                                    maxBytesPerSec, minBytesPerSec, 
                                    avgBytesPerSec, totalDuration);
        }
        catch (...) {
            debugPrint("Exception during logging");
            printStack();
        }
    }
    
    debugPrint("Copy operation complete");
    exitFunction("copyFile");
}