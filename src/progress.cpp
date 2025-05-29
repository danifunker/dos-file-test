#include "progress.h"
#include <iostream.h>
#include <iomanip.h>

void Progress::showProgressBar(long bytesTransferred, long totalBytes, double speed) {
    // Make sure we don't divide by zero
    if (totalBytes <= 0) {
        cout << "Progress: 0% complete      \r";
        cout.flush();
        return;
    }
    
    // Calculate percentage, being careful with integer division
    // Use long division to avoid overflow with large files
    long percent = (bytesTransferred * 100L) / totalBytes;
    
    // Cap at 100% to avoid showing more than 100%
    if (percent > 100) percent = 100;
    
    cout << "Progress: " << percent << "% complete";
    
    // Only show speed if it's valid
    if (speed > 0) {
        long kbSpeed = (long)(speed / 1024.0);
        cout << " - " << kbSpeed << " KB/sec";
    }
    
    cout << "      \r";
    cout.flush();
}