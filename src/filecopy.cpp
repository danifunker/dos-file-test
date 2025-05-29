#include <iostream.h>
#include <fstream.h>
#include <string.h>
#include <time.h>
#include "filecopy.h"
#include "progress.h"
#include "logger.h"

void FileCopy::copyFile(const char* sourcePath, const char* destPath) {
    ifstream sourceFile(sourcePath, ios::binary);
    ofstream destFile(destPath, ios::binary);

    if (!sourceFile) {
        cerr << "Error opening source file: " << sourcePath << endl;
        return;
    }
    if (!destFile) {
        cerr << "Error opening destination file: " << destPath << endl;
        return;
    }

    sourceFile.seekg(0, ios::end);
    long fileSize = sourceFile.tellg();
    sourceFile.seekg(0, ios::beg);

    char buffer[4096];
    long bytesRead;
    time_t startTime = time(NULL);
    long totalBytesCopied = 0;
    double maxSpeed = 0.0, minSpeed = 0.0, totalSpeed = 0.0;
    int speedCount = 0;

    Progress progress;
    progress.showProgressBar(0, fileSize, 0.0);

    while (sourceFile) {
        sourceFile.read(buffer, sizeof(buffer));
        bytesRead = sourceFile.gcount();
        if (bytesRead <= 0) break;
        
        destFile.write(buffer, bytesRead);
        totalBytesCopied += bytesRead;

        time_t currentTime = time(NULL);
        double duration = difftime(currentTime, startTime);
        double currentSpeed = (duration > 0) ? (totalBytesCopied / duration) : 0.0;

        if (speedCount == 0) {
            maxSpeed = minSpeed = currentSpeed;
        } else {
            if (currentSpeed > maxSpeed) maxSpeed = currentSpeed;
            if (currentSpeed < minSpeed) minSpeed = currentSpeed;
        }

        totalSpeed += currentSpeed;
        speedCount++;

        progress.showProgressBar(totalBytesCopied, fileSize, currentSpeed);
    }

    time_t endTime = time(NULL);
    double totalDuration = difftime(endTime, startTime);
    double averageSpeed = (speedCount > 0) ? (totalSpeed / speedCount) : 0.0;

    Logger logger;
    logger.logTransferDetails(sourcePath, destPath, fileSize, maxSpeed, minSpeed, averageSpeed, totalDuration);

    sourceFile.close();
    destFile.close();
}