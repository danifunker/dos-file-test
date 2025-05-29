#include <iostream.h>
#include <string.h>
#include <dos.h>
#include <dir.h>
#include "filecopy.h"

#define VERSION "0.1"

void showUsage(const char* programName) {
    cout << "Usage: " << programName << " <source_file> <destination_file>" << endl;
    cout << endl;
    cout << "Parameters:" << endl;
    cout << "  <source_file>      - Path to the file to be copied" << endl;
    cout << "  <destination_folder> - Path where the file will be copied to" << endl;
    cout << endl;
    cout << "Examples: " << endl;
    cout << "  " << programName << " C:\\DATA.TXT D:\\BACKUP.TXT" << endl;
    cout << "  " << programName << " DATA.TXT BACKUP.TXT" << endl;
    cout << "  " << programName << " ..\\SOURCE\\DATA.TXT ..\\DEST\\DATA.TXT" << endl;
}

int main(int argc, char* argv[]) {
    cout << "FileCopy Utility v" << VERSION << endl;
    cout << "GitHub: https://github.com/danifunker/dos-file-test" << endl;
    cout << endl;

    char sourcePath[MAXPATH] = {0};
    char destinationPath[MAXPATH] = {0};

    // Process command line arguments
    if (argc < 3) {
        showUsage(argv[0]);
        return 1;
    }
    
    // Copy arguments to our buffers
    strcpy(sourcePath, argv[1]);
    strcpy(destinationPath, argv[2]);
    
    // Get absolute paths if needed
    if (sourcePath[0] != '\\' && sourcePath[1] != ':') {
        char temp[MAXPATH];
        getcwd(temp, MAXPATH);
        strcat(temp, "\\");
        strcat(temp, sourcePath);
        strcpy(sourcePath, temp);
    }
    
    if (destinationPath[0] != '\\' && destinationPath[1] != ':') {
        char temp[MAXPATH];
        getcwd(temp, MAXPATH);
        strcat(temp, "\\");
        strcat(temp, destinationPath);
        strcpy(destinationPath, temp);
    }
    
    cout << "Source: " << sourcePath << endl;
    cout << "Destination: " << destinationPath << endl;
    cout << endl;

    FileCopy fileCopy;
    fileCopy.copyFile(sourcePath, destinationPath);
    
    cout << "File transfer operation completed." << endl;
    return 0;
}