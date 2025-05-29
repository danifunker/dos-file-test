#include "progress.h"
#include <iostream.h>
#include <iomanip.h>

void Progress::showProgressBar(long bytesTransferred, long totalBytes, double speed) {
    const int barWidth = 50; // Reduced for DOS compatibility
    double progress = (double)bytesTransferred / totalBytes;
    int pos = (int)(barWidth * progress);

    cout << "[";
    for (int i = 0; i < barWidth; ++i) {
        if (i < pos) cout << "=";
        else if (i == pos) cout << ">";
        else cout << " ";
    }
    // Show progress percentage with 2 decimal places
    cout << "] " << setprecision(2) << setiosflags(ios::fixed) 
         << (progress * 100.0) << " %\r";
    cout.flush();

    // Convert bytes/sec to MB/sec and display with 2 decimal places
    double speedInMB = speed / (1024.0 * 1024.0); // Convert to MB/s
    cout << "\nCurrent Speed: " << setprecision(2) 
         << setiosflags(ios::fixed) << speedInMB << " MB/sec" << endl;
}