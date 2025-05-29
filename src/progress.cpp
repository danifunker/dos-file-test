#include "progress.h"
#include <iostream.h>
#include <iomanip.h>
#include <stdio.h>  // For sprintf
#include <string.h> // For strcpy

// Function to format time remaining in minutes:seconds
void formatTimeRemaining(long secondsRemaining, char* buffer) {
    if (secondsRemaining < 0) {
        strcpy(buffer, "unknown");
        return;
    }
    
    long minutes = secondsRemaining / 60;
    long seconds = secondsRemaining % 60;
    
    if (minutes > 0) {
        sprintf(buffer, "%ldm %lds", minutes, seconds);
    } else {
        sprintf(buffer, "%lds", seconds);
    }
}

// Function to format speed in appropriate units with 2 decimal places
void formatSpeed(long bytesPerSec, char* buffer) {
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

// Changed to use longs instead of double for speed
void Progress::showProgressBar(long bytesTransferred, long totalBytes, long bytesPerSec) {
    // Make sure we don't divide by zero
    if (totalBytes <= 0) {
        cout << "Progress: 0% complete      \r";
        cout.flush();
        return;
    }
    
    // Calculate percentage safely to avoid integer overflow
    // First divide each term to keep values in range
    long percent;
    if (totalBytes > 10000000L) {
        // For large files, scale down to avoid overflow
        percent = (bytesTransferred / 1024) * 100L / (totalBytes / 1024);
    } else {
        // For smaller files, use the original calculation
        percent = (bytesTransferred * 100L) / totalBytes;
    }
    
    // Cap at 100% to avoid showing more than 100%
    if (percent > 100) percent = 100;
    
    // Calculate time remaining (in seconds)
    long secondsRemaining = -1;
    if (bytesPerSec > 0 && bytesTransferred < totalBytes) {
        long bytesLeft = totalBytes - bytesTransferred;
        secondsRemaining = bytesLeft / bytesPerSec;
    }
    
    char timeRemainingStr[20];
    formatTimeRemaining(secondsRemaining, timeRemainingStr);
    
    char speedStr[20];
    formatSpeed(bytesPerSec, speedStr);
    
    cout << "Progress: " << percent << "% complete";
    
    // Only show speed and time remaining if we have valid data
    if (bytesPerSec > 0) {
        cout << " - " << speedStr << " - " << timeRemainingStr << " remaining";
    }
    
    cout << "      \r";
    cout.flush();
}