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