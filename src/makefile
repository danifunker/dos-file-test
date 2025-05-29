# Makefile for Borland C++ 5.02 DOS application

# Model configuration (DOS16 large model)
SYSTEM = DOS16
MODEL = l

# Source files
EXE = filecopy
OBJEXE = src\main.obj src\filecopy.obj src\progress.obj src\logger.obj

# Compiler settings
CPUOPT = 3
CFLAGS = -ml -w -O1
CCFEXE = -I\src

.autodepend

# Default rule - build all
all: $(EXE).exe

# Individual object files
src\main.obj: src\main.cpp
    $(MAKEDIR)\bin\bcc $(CFLAGS) -c src\main.cpp

src\filecopy.obj: src\filecopy.cpp src\filecopy.h
    $(MAKEDIR)\bin\bcc $(CFLAGS) -c src\filecopy.cpp

src\progress.obj: src\progress.cpp src\progress.h
    $(MAKEDIR)\bin\bcc $(CFLAGS) -c src\progress.cpp

src\logger.obj: src\logger.cpp src\logger.h
    $(MAKEDIR)\bin\bcc $(CFLAGS) -c src\logger.cpp

# Link the executable
$(EXE).exe: $(OBJEXE)
    $(MAKEDIR)\bin\bcc $(CFLAGS) -e$(EXE).exe $(OBJEXE)

# Clean target
clean:
    del src\*.obj
    del $(EXE).exe