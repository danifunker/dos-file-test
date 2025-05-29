#ifndef FILECOPY_H
#define FILECOPY_H

class FileCopy {
public:
    void copyFile(const char* sourcePath, const char* destPath);
    
    // Set this to true to overwrite files without prompting
    bool forceOverwrite;
    
    FileCopy() : forceOverwrite(false) {}
};

#endif // FILECOPY_H