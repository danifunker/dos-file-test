#include "progress.h"
#include <iostream.h>
#include <iomanip.h>

// Changed to use longs instead of double for speed
void Progress::showProgressBar(long bytesTransferred, long totalBytes, long bytesPerSec) {
    // Make sure we don't divide by zero
    if (totalBytes <= 0) {
        cout << "Progress: 0% complete      \r";
        cout.flush();
        return;
    }
    
    // Calculate percentage with integer division
    long percent = (bytesTransferred * 100L) / totalBytes;
    
    // Cap at 100% to avoid showing more than 100%
    if (percent > 100) percent = 100;
    
    cout << "Progress: " << percent << "% complete";
    
    // Only show speed if it's valid
    if (bytesPerSec > 0) {
        long kbSpeed = bytesPerSec / 1024;
        cout << " - " << kbSpeed << " KB/sec";
    }
    
    cout << "      \r";
    cout.flush();
}