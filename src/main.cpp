/* 
 * FileCopy Utility
 * Copyright (C) 2025 Dani Sarfati (danifunker)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 * This file contains code adapted from the FreeDOS xcopy utility
 * Source: https://github.com/FDOS/xcopy/tree/master/source
 */

#include <iostream.h>
#include <string.h>
#include <dos.h>
#include <dir.h>
#include <stdio.h>
#include <ctype.h>
#include "filecopy.h"

#define VERSION "0.6"

// Flags for operation modes
bool forceOverwrite = false;
bool debugMode = false;  // New flag for debug mode

void showUsage(const char* programName) {
    cout << "FileCopy Utility v" << VERSION << endl;
    cout << "Copyright (C) 2025 Dani Sarfati (danifunker)" << endl;
    cout << "Licensed under GNU GPL v2 or later" << endl;
    cout << "GitHub: https://github.com/danifunker/dos-file-test" << endl;
    cout << endl;
    cout << "Usage: " << programName << " <source_file> <destination_file> [options]" << endl;
    cout << endl;
    cout << "Parameters:" << endl;
    cout << "  <source_file>      - Path to the file to be copied" << endl;
    cout << "  <destination_file> - Path where the file will be copied to" << endl;
    cout << endl;
    cout << "Options:" << endl;
    cout << "  /y                 - Overwrite files without prompting" << endl;
    cout << "  /d                 - Show debug information" << endl;
    cout << endl;
    cout << "Examples: " << endl;
    cout << "  " << programName << " C:\\DATA.TXT D:\\BACKUP.TXT" << endl;
    cout << "  " << programName << " DATA.TXT BACKUP.TXT" << endl;
    cout << "  " << programName << " ..\\SOURCE\\DATA.TXT ..\\DEST\\DATA.TXT /y" << endl;
}

// Function to check if a file exists
bool fileExists(const char* filePath) {
    FILE* file = fopen(filePath, "r");
    if (file) {
        fclose(file);
        return true;
    }
    return false;
}

// Function to get file size
long getFileSize(const char* filePath) {
    FILE* file = fopen(filePath, "rb");
    if (!file) return 0;
    
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fclose(file);
    return size;
}

// Function to format file size with MB and bytes
void formatFileSize(long sizeInBytes, char* buffer) {
    // Calculate size in MB with 2 decimal places
    double sizeInMB = (double)sizeInBytes / (1024.0 * 1024.0);
    sprintf(buffer, "%.2f MB [%ld bytes]", sizeInMB, sizeInBytes);
}

// Function to prompt for file overwrite
bool promptOverwrite(const char* filePath, const char* sourcePath) {
    long destSize = getFileSize(filePath);
    long sourceSize = getFileSize(sourcePath);
    
    char destSizeStr[50];
    char sourceSizeStr[50];
    
    formatFileSize(destSize, destSizeStr);
    formatFileSize(sourceSize, sourceSizeStr);
    
    cout << "File already exists: " << filePath << endl;
    cout << "Destination size: " << destSizeStr << endl;
    cout << "Source size:      " << sourceSizeStr << endl;
    cout << "Overwrite? (Y)es/(N)o/(A)ll: ";
    
    char response;
    cin >> response;
    response = toupper(response);
    
    if (response == 'A') {
        forceOverwrite = true;
        return true;
    }
    
    return (response == 'Y');
}

// Add this function to check if a path is a directory
bool isDirectory(const char* path) {
    struct ffblk fileInfo;
    int result = findfirst(path, &fileInfo, FA_DIREC);
    
    if (result != 0) {
        return false;  // Path doesn't exist or other error
    }
    
    return (fileInfo.ff_attrib & FA_DIREC) != 0;
}

// Add this function to extract the filename from a path
void extractFilename(const char* path, char* filename) {
    const char* lastSlash = strrchr(path, '\\');
    const char* lastFwdSlash = strrchr(path, '/');
    
    // Find the last directory separator
    const char* lastSep = lastSlash;
    if (lastFwdSlash > lastSlash) {
        lastSep = lastFwdSlash;
    }
    
    if (lastSep) {
        strcpy(filename, lastSep + 1);
    } else {
        strcpy(filename, path);  // No separator found, use the whole path
    }
}

// Modify the main function to handle directory destinations
int main(int argc, char* argv[]) {
    cout << "FileCopy Utility v" << VERSION << endl;
    cout << "GitHub: https://github.com/danifunker/dos-file-test" << endl;
    cout << endl;

    char sourcePath[MAXPATH] = {0};
    char destinationPath[MAXPATH] = {0};
    char finalDestPath[MAXPATH] = {0};  // Will store the final destination path

    // Process command line arguments
    if (argc < 3) {
        showUsage(argv[0]);
        return 1;
    }
    
    // Copy arguments to our buffers
    strcpy(sourcePath, argv[1]);
    strcpy(destinationPath, argv[2]);
    
    // Check for flags
    for (int i = 3; i < argc; i++) {
        if (stricmp(argv[i], "/y") == 0) {
            forceOverwrite = true;
        }
        else if (stricmp(argv[i], "/d") == 0) {
            debugMode = true;
        }
    }
    
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
    
    // Check if source file exists
    if (!fileExists(sourcePath)) {
        cerr << "Error: Source file does not exist: " << sourcePath << endl;
        return 1;
    }
    
    // Check if destination is a directory
    strcpy(finalDestPath, destinationPath);  // Start with the provided destination
    
    if (isDirectory(destinationPath)) {
        char sourceFilename[MAXPATH];
        extractFilename(sourcePath, sourceFilename);
        
        // Ensure destination path ends with a backslash
        int destLen = strlen(destinationPath);
        if (destinationPath[destLen-1] != '\\' && destinationPath[destLen-1] != '/') {
            strcat(finalDestPath, "\\");
        }
        
        // Append the source filename to the destination directory
        strcat(finalDestPath, sourceFilename);
        
        cout << "Destination is a directory, using: " << finalDestPath << endl;
    }
    
    cout << "Source: " << sourcePath << endl;
    cout << "Destination: " << finalDestPath << endl;
    cout << endl;
    
    // Check if destination file exists and prompt for overwrite if needed
    if (fileExists(finalDestPath) && !forceOverwrite) {
        if (!promptOverwrite(finalDestPath, sourcePath)) {
            cout << "Copy operation cancelled." << endl;
            return 0;
        }
    }

    FileCopy fileCopy;
    fileCopy.setDebugMode(debugMode);  // Pass debug mode to FileCopy
    fileCopy.copyFile(sourcePath, finalDestPath);
    
    cout << "File transfer operation completed." << endl;
    return 0;
}