#ifndef PROGRESS_H
#define PROGRESS_H

class Progress {
public:
    void showProgressBar(long bytesTransferred, long totalBytes, double speed);
};

#endif // PROGRESS_H