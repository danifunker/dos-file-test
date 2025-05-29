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
    cout << "] " << (int)(progress * 100.0) << " %\r";
    cout.flush();

    cout << "\nCurrent Speed: " << setprecision(2) << fixed << speed << " bytes/sec" << endl;
}