#include <iostream.h>
#include <string.h>
#include "filecopy.h"

#define VERSION "0.1"

void showUsage(const char* programName) {
    cout << "FileCopy Utility v" << VERSION << endl;
    cout << "GitHub: https://github.com/danifunker/dos-file-test" << endl;
    cout << endl;
    cout << "Usage: " << programName << " <source_file> <destination_file>" << endl;
    cout << endl;
    cout << "Parameters:" << endl;
    cout << "  <source_file>      - Path to the file to be copied" << endl;
    cout << "  <destination_file> - Path where the file will be copied to" << endl;
    cout << endl;
    cout << "Example: " << programName << " C:\\DATA.TXT D:\\BACKUP.TXT" << endl;
}

int main(int argc, char* argv[]) {
    cout << "FileCopy Utility v" << VERSION << endl;
    cout << "GitHub: https://github.com/danifunker/dos-file-test" << endl;
    cout << endl;

    char sourcePath[256] = {0};
    char destinationPath[256] = {0};

    // Process command line arguments
    if (argc < 3) {
        showUsage(argv[0]);
        return 1;
    }
    
    // Copy arguments to our buffers
    strncpy(sourcePath, argv[1], 255);
    strncpy(destinationPath, argv[2], 255);
    
    cout << "Source: " << sourcePath << endl;
    cout << "Destination: " << destinationPath << endl;
    cout << endl;

    FileCopy fileCopy;
    fileCopy.copyFile(sourcePath, destinationPath);
    
    cout << "File transfer operation completed." << endl;
    return 0;
}