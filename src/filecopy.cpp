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
#include <signal.h>  // For signal handling
#include "filecopy.h"
#include "progress.h"
#include "logger.h"

// Global variables for signal handling
static int gSourceHandle = -1;
static int gDestHandle = -1;
static const char* gSourcePath = NULL;
static const char* gDestPath = NULL;
static long gFileSize = 0;
static long gTotalBytesCopied = 0;
static time_t gStartTime = 0;
static bool gDebugMode = false;

// Signal handler for CTRL+C
void interruptHandler(int sig) {
    cout << "\n\nCTRL+C pressed. Interrupting copy operation..." << endl;
    
    // Record current time for timeout calculations
    time_t interruptTime = time(NULL);
    const int MAX_CLOSE_WAIT_SECONDS = 2; // Maximum time to wait for file close
    bool sourceClosedOk = false;
    bool destClosedOk = false;
    
    // Attempt to close open file handles with timeout
    if (gSourceHandle >= 0) {
        cout << "Closing source file..." << endl;
        int closeResult = close(gSourceHandle);
        sourceClosedOk = (closeResult == 0);
        if (!sourceClosedOk) {
            cerr << "Warning: Could not close source file cleanly." << endl;
        }
    }
    
    // Try to close destination even if source failed
    if (gDestHandle >= 0) {
        cout << "Closing destination file..." << endl;
        int closeResult = close(gDestHandle);
        destClosedOk = (closeResult == 0);
        if (!destClosedOk) {
            cerr << "Warning: Could not close destination file cleanly." << endl;
        }
    }
    
    // Check if we've exceeded our timeout
    time_t currentTime = time(NULL);
    if (difftime(currentTime, interruptTime) > MAX_CLOSE_WAIT_SECONDS) {
        cout << "Timeout waiting for files to close. Forcing termination." << endl;
    }
    
    // If we couldn't close files, mark handles as invalid anyway
    // to prevent double-close attempts later
    gSourceHandle = -1;
    gDestHandle = -1;
    
    // Calculate elapsed time and speed
    time_t endTime = time(NULL);
    long totalDuration = (long)difftime(endTime, gStartTime);
    
    // Display stats about the interrupted transfer
    cout << "Transfer interrupted after copying " << gTotalBytesCopied 
         << " of " << gFileSize << " bytes (" 
         << (gTotalBytesCopied * 100 / (gFileSize ? gFileSize : 1)) << "%)" << endl;
    
    // Calculate speed
    long avgBytesPerSec = 0;
    if (totalDuration > 0) {
        avgBytesPerSec = gTotalBytesCopied / totalDuration;
    }
    
    cout << "Time elapsed: " << totalDuration << " seconds" << endl;
    cout << "Average speed: " << (avgBytesPerSec / 1024) << " KB/sec" << endl;
    
    // Write a quick log file to record the interrupted transfer
    try {
        if (gSourcePath && gDestPath) {
            // Create a special interrupted log entry
            char interruptedLogPath[256];
            strcpy(interruptedLogPath, "INTERUPT.LOG");  // Using a different name for interrupted transfers
            
            ofstream logFile(interruptedLogPath, ios::app);
            if (logFile) {
                // Get current time
                time_t now = time(NULL);
                struct tm* localTime = localtime(&now);
                char timeBuffer[80];
                strftime(timeBuffer, 80, "%Y-%m-%d %H:%M:%S", localTime);
                
                logFile << "INTERRUPTED TRANSFER LOG" << endl;
                logFile << "Date and Time: " << timeBuffer << endl;
                logFile << "Source: " << gSourcePath << endl;
                logFile << "Destination: " << gDestPath << endl;
                logFile << "Total file size: " << gFileSize << " bytes" << endl;
                logFile << "Bytes copied: " << gTotalBytesCopied << " bytes" << endl;
                logFile << "Completion: " << (gTotalBytesCopied * 100 / (gFileSize ? gFileSize : 1)) << "%" << endl;
                logFile << "Avg: " << (avgBytesPerSec / 1024) << " KB/sec" << endl;
                logFile << "Time: " << totalDuration << " seconds" << endl;
                logFile << "Status: INTERRUPTED BY USER (CTRL+C)" << endl;
                if (!sourceClosedOk || !destClosedOk) {
                    logFile << "Note: One or more files could not be closed cleanly" << endl;
                }
                logFile << "----------------------------------------" << endl;
                
                logFile.close();
                cout << "Interrupted transfer log written to INTERUPT.LOG" << endl;
            }
        }
    }
    catch (...) {
        cerr << "Error writing interrupt log file." << endl;
    }
    
    cout << "Copy operation terminated by user." << endl;
    exit(1);  // Exit the program immediately
}

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
    
    // Set up signal handler for CTRL+C
    gSourcePath = sourcePath;
    gDestPath = destPath;
    gDebugMode = m_debugMode;
    signal(SIGINT, interruptHandler);
    
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
    gSourceHandle = open(normalizedSource, O_RDONLY | O_BINARY);
    if (gSourceHandle < 0) {
        cerr << "Error opening source file: " << normalizedSource << endl;
        exitFunction("copyFile", m_debugMode);
        return;
    }
    
    debugPrint("Getting file size", m_debugMode);
    // Get file size using filelength() which is more reliable in DOS
    gFileSize = filelength(gSourceHandle);
    
    static char sizeMsg[80];
    sprintf(sizeMsg, "File size: %ld bytes", gFileSize);
    debugPrint(sizeMsg, m_debugMode);
    
    debugPrint("Opening destination file", m_debugMode);
    // Open destination file - use 0666 for permission (rw-rw-rw-)
    gDestHandle = open(normalizedDest, O_WRONLY | O_CREAT | O_TRUNC | O_BINARY, 0666);
    if (gDestHandle < 0) {
        cerr << "Error opening destination file: " << normalizedDest << endl;
        close(gSourceHandle);
        gSourceHandle = -1;
        exitFunction("copyFile", m_debugMode);
        return;
    }
    
    // Buffer size - 8KB is a good balance for DOS
    const int BUFFER_SIZE = 8192;
    debugPrint("Allocating buffer", m_debugMode);
    static char buffer[BUFFER_SIZE]; // Static to avoid stack issues
    
    gStartTime = time(NULL);
    gTotalBytesCopied = 0;
    
    // Speed tracking variables
    long maxBytesPerSec = 0;
    long minBytesPerSec = 0;  // Initialize to 0 instead of max long
    bool speedInitialized = false;
    int speedCount = 0;
    
    debugPrint("Setting up progress tracking", m_debugMode);
    time_t lastUpdateTime = gStartTime;
    
    cout << "Starting copy operation..." << endl;
    cout << "Press CTRL+C to interrupt the transfer at any time." << endl;
    
    // Initial progress display
    Progress progress;
    debugPrint("Showing initial progress", m_debugMode);
    progress.showProgressBar(0, gFileSize, 0);
    
    debugPrint("Starting copy loop", m_debugMode);
    
    int bytesRead;
    long loopCount = 0;
    bool error = false;
    
    // Increase update frequency - update every 0.2 seconds
    const double UPDATE_INTERVAL = 0.2;
    
    // Copy in chunks
    while ((bytesRead = read(gSourceHandle, buffer, BUFFER_SIZE)) > 0) {
        loopCount++;
        
        // Debug output for every 100th iteration
        if (m_debugMode && loopCount % 100 == 0) {
            static char loopMsg[80];
            sprintf(loopMsg, "Copy loop iteration: %ld, Bytes copied: %ld", loopCount, gTotalBytesCopied);
            debugPrint(loopMsg, m_debugMode);
        }
        
        // Write the chunk to destination
        int bytesWritten = write(gDestHandle, buffer, bytesRead);
        if (bytesWritten != bytesRead) {
            cerr << "Error writing to destination file" << endl;
            debugPrint("Write error, closing files", m_debugMode);
            error = true;
            break;
        }
        
        gTotalBytesCopied += bytesRead;
        
        // Update progress display more frequently (every 0.2 seconds)
        time_t currentTime = time(NULL);
        if (difftime(currentTime, lastUpdateTime) >= UPDATE_INTERVAL) {
            debugPrint("Updating progress display", m_debugMode);
            long elapsedSecs = (long)difftime(currentTime, gStartTime);
            
            // Calculate speed (bytes per second)
            long currentBytesPerSec = 0;
            if (elapsedSecs > 0) {
                currentBytesPerSec = gTotalBytesCopied / elapsedSecs;
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
            progress.showProgressBar(gTotalBytesCopied, gFileSize, currentBytesPerSec);
            
            lastUpdateTime = currentTime;
        }
        
        // Let DOS breathe occasionally
        if (loopCount % 20 == 0) {
            delay(1);
        }
    }
    
    debugPrint("Copy loop complete, closing files", m_debugMode);
    // Close both files
    close(gSourceHandle);
    close(gDestHandle);
    gSourceHandle = gDestHandle = -1;
    
    if (error) {
        exitFunction("copyFile", m_debugMode);
        return;
    }
    
    debugPrint("Calculating final statistics", m_debugMode);
    time_t endTime = time(NULL);
    long totalDuration = (long)difftime(endTime, gStartTime);
    
    long avgBytesPerSec = 0;
    if (totalDuration > 0) {
        avgBytesPerSec = gTotalBytesCopied / totalDuration;
    }
    
    debugPrint("Displaying completion message", m_debugMode);
    cout << "\nCopy complete: " << gFileSize << " bytes in " 
         << totalDuration << " seconds" << endl;
         
    long avgKBPerSec = avgBytesPerSec / 1024;
    cout << "Average speed: " << avgKBPerSec << " KB/sec" << endl;
    
    if (gTotalBytesCopied == gFileSize) {
        debugPrint("Writing log file", m_debugMode);
        Logger logger;
        logger.logTransferDetails(sourcePath, destPath, gFileSize, 
                                maxBytesPerSec, minBytesPerSec, 
                                avgBytesPerSec, totalDuration);
    }
    
    // Reset signal handler to default
    signal(SIGINT, SIG_DFL);
    
    debugPrint("Copy operation complete", m_debugMode);
    exitFunction("copyFile", m_debugMode);
}