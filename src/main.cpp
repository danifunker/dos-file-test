#include <iostream.h>  // Note: no .h for modern headers, but .h for old-style headers
#include <string.h>
#include "filecopy.h"

int main() {
    char sourcePath[256];    // Use fixed-size character arrays instead of std::string
    char destinationPath[256];

    cout << "Enter the source file path: ";
    cin.getline(sourcePath, 256);  // Use C-style getline
    
    cout << "Enter the destination file path: ";
    cin.getline(destinationPath, 256);

    FileCopy fileCopy;
    fileCopy.copyFile(sourcePath, destinationPath);
    
    cout << "File transfer operation completed." << endl;
    return 0;
}