# Makefile for Borland C++ 5.02 DOS application

# Model configuration (DOS16 large model)
SYSTEM = DOS16
MODEL = l

# Source files
EXE = filecopy
OBJEXE = main.obj filecopy.obj progress.obj logger.obj

# Compiler settings
CPUOPT = 3
CFLAGS = -ml -w -O1
CCFEXE = -I

.autodepend

# Default rule - build all
all: $(EXE).exe

# Individual object files
main.obj: main.cpp
    bcc $(CFLAGS) -c main.cpp

filecopy.obj: filecopy.cpp filecopy.h
    bcc $(CFLAGS) -c filecopy.cpp

progress.obj: progress.cpp progress.h
    bcc $(CFLAGS) -c progress.cpp

logger.obj: logger.cpp logger.h
    bcc $(CFLAGS) -c logger.cpp

# Link the executable
$(EXE).exe: $(OBJEXE)
    bcc $(CFLAGS) -e$(EXE).exe $(OBJEXE)

# Clean target
clean:
    del *.obj
    del $(EXE).exe