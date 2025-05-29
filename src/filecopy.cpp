/* 
 * FileCopy Utility
 * Copyright (C) 2025 Dani Sarfati (danifunker)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
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
 * This file contains code adapted from the FreeDOS xcopy utility
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

// Simple stack tracking for debugging
int stackDepth = 0;
char stackTrace[10][80]; // Store up to 10 function calls

void enterFunction(const char* funcName, bool debugMode) {
    if (stackDepth < 10) {
        sprintf(stackTrace[stackDepth], "-> %s", funcName);
        stackDepth++;
    }
    
    if (debugMode) {
        cout << "[DEBUG] Entering: " << funcName << endl;
        cout << "[DEBUG] Stack depth: " << stackDepth << endl;
    }
}

void exitFunction(const char* funcName, bool debugMode) {
    if (stackDepth > 0) {
        stackDepth--;
    }
    
    if (debugMode) {
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
void debugPrint(const char* message, bool debugMode) {
    if (debugMode) {
        cout << "[DEBUG] " << message << endl;
    }
}

// Normalize a file path to ensure it's valid for DOS
void normalizePath(char* path, bool debugMode) {
    enterFunction("normalizePath", debugMode);
    
    // Convert forward slashes to backslashes
    for (int i = 0; path[i] != '\0'; i++) {
        if (path[i] == '/') {
            path[i] = '\\';
        }
    }
    
    exitFunction("normalizePath", debugMode);
}

// This implementation is based on FreeDOS xcopy's direct file copy mechanism
void FileCopy::copyFile(const char* sourcePath, const char* destPath) {
    enterFunction("copyFile", m_debugMode);
    debugPrint("Copy operation started", m_debugMode);
    
    // Use static buffers to reduce stack usage
    static char normalizedSource[256];
    static char normalizedDest[256];
    
    // Make copies so we can safely modify them
    strcpy(normalizedSource, sourcePath);
    strcpy(normalizedDest, destPath);
    
    // Convert slashes for DOS compatibility
    normalizePath(normalizedSource, m_debugMode);
    normalizePath(normalizedDest, m_debugMode);
    
    debugPrint("Opening source file", m_debugMode);
    
    // Open source file using low-level file I/O
    int sourceHandle = open(normalizedSource, O_RDONLY | O_BINARY);
    if (sourceHandle < 0) {
        cerr << "Error opening source file: " << normalizedSource << endl;
        exitFunction("copyFile", m_debugMode);
        return;
    }
    
    debugPrint("Getting file size", m_debugMode);
    // Get file size using filelength() which is more reliable in DOS
    long fileSize = filelength(sourceHandle);
    
    static char sizeMsg[80];
    sprintf(sizeMsg, "File size: %ld bytes", fileSize);
    debugPrint(sizeMsg, m_debugMode);
    
    debugPrint("Opening destination file", m_debugMode);
    // Open destination file - use 0666 for permission (rw-rw-rw-)
    int destHandle = open(normalizedDest, O_WRONLY | O_CREAT | O_TRUNC | O_BINARY, 0666);
    if (destHandle < 0) {
        cerr << "Error opening destination file: " << normalizedDest << endl;
        close(sourceHandle);
        exitFunction("copyFile", m_debugMode);
        return;
    }
    
    // Buffer size - 8KB is a good balance for DOS
    const int BUFFER_SIZE = 8192;
    debugPrint("Allocating buffer", m_debugMode);
    static char buffer[BUFFER_SIZE]; // Static to avoid stack issues
    
    time_t startTime = time(NULL);
    long totalBytesCopied = 0;
    
    // Speed tracking variables
    long maxBytesPerSec = 0;
    long minBytesPerSec = 0;  // Initialize to 0 instead of max long
    bool speedInitialized = false;
    int speedCount = 0;
    
    debugPrint("Setting up progress tracking", m_debugMode);
    time_t lastUpdateTime = startTime;
    
    cout << "Starting copy operation..." << endl;
    
    // Initial progress display
    Progress progress;
    debugPrint("Showing initial progress", m_debugMode);
    progress.showProgressBar(0, fileSize, 0);
    
    debugPrint("Starting copy loop", m_debugMode);
    
    int bytesRead;
    long loopCount = 0;
    bool error = false;
    
    // Increase update frequency - update every 0.2 seconds
    const double UPDATE_INTERVAL = 0.2;
    
    // Copy in chunks
    while ((bytesRead = read(sourceHandle, buffer, BUFFER_SIZE)) > 0) {
        loopCount++;
        
        // Debug output for every 100th iteration
        if (m_debugMode && loopCount % 100 == 0) {
            static char loopMsg[80];
            sprintf(loopMsg, "Copy loop iteration: %ld, Bytes copied: %ld", loopCount, totalBytesCopied);
            debugPrint(loopMsg, m_debugMode);
        }
        
        // Write the chunk to destination
        int bytesWritten = write(destHandle, buffer, bytesRead);
        if (bytesWritten != bytesRead) {
            cerr << "Error writing to destination file" << endl;
            debugPrint("Write error, closing files", m_debugMode);
            error = true;
            break;
        }
        
        totalBytesCopied += bytesRead;
        
        // Update progress display more frequently (every 0.2 seconds)
        time_t currentTime = time(NULL);
        if (difftime(currentTime, lastUpdateTime) >= UPDATE_INTERVAL) {
            debugPrint("Updating progress display", m_debugMode);
            long elapsedSecs = (long)difftime(currentTime, startTime);
            
            // Calculate speed (bytes per second)
            long currentBytesPerSec = 0;
            if (elapsedSecs > 0) {
                currentBytesPerSec = totalBytesCopied / elapsedSecs;
            }
            
            // Update min/max speed - integer only
            if (currentBytesPerSec > 0) {
                if (!speedInitialized) {
                    maxBytesPerSec = minBytesPerSec = currentBytesPerSec;
                    speedInitialized = true;
                } else {
                    if (currentBytesPerSec > maxBytesPerSec) {
                        maxBytesPerSec = currentBytesPerSec;
                    }
                    if (currentBytesPerSec < minBytesPerSec) {
                        minBytesPerSec = currentBytesPerSec;
                    }
                }
                speedCount++;
            }
            
            // Update progress display
            progress.showProgressBar(totalBytesCopied, fileSize, currentBytesPerSec);
            
            lastUpdateTime = currentTime;
        }
        
        // Let DOS breathe occasionally
        if (loopCount % 20 == 0) {
            delay(1);
        }
    }
    
    debugPrint("Copy loop complete, closing files", m_debugMode);
    // Close both files
    close(sourceHandle);
    close(destHandle);
    
    if (error) {
        exitFunction("copyFile", m_debugMode);
        return;
    }
    
    debugPrint("Calculating final statistics", m_debugMode);
    time_t endTime = time(NULL);
    long totalDuration = (long)difftime(endTime, startTime);
    
    long avgBytesPerSec = 0;
    if (totalDuration > 0) {
        avgBytesPerSec = totalBytesCopied / totalDuration;
    }
    
    debugPrint("Displaying completion message", m_debugMode);
    cout << "\nCopy complete: " << fileSize << " bytes in " 
         << totalDuration << " seconds" << endl;
         
    long avgKBPerSec = avgBytesPerSec / 1024;
    cout << "Average speed: " << avgKBPerSec << " KB/sec" << endl;
    
    if (totalBytesCopied == fileSize) {
        debugPrint("Writing log file", m_debugMode);
        Logger logger;
        logger.logTransferDetails(sourcePath, destPath, fileSize, 
                                maxBytesPerSec, minBytesPerSec, 
                                avgBytesPerSec, totalDuration);
    }
    
    debugPrint("Copy operation complete", m_debugMode);
    exitFunction("copyFile", m_debugMode);
}