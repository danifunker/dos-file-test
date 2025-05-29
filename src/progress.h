#ifndef PROGRESS_H
#define PROGRESS_H

class Progress {
public:
    // Changed to use longs instead of double for speed
    void showProgressBar(long bytesTransferred, long totalBytes, long bytesPerSec);
};

#endif // PROGRESS_H