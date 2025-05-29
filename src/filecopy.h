#ifndef FILECOPY_H
#define FILECOPY_H

class FileCopy {
private:
    bool m_debugMode;
    
public:
    void copyFile(const char* sourcePath, const char* destPath);
    
    // Set debug mode
    void setDebugMode(bool mode) { m_debugMode = mode; }
    bool getDebugMode() const { return m_debugMode; }
    
    // Constructor
    FileCopy() : m_debugMode(false) {}
};

#endif // FILECOPY_H