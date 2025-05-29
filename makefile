# Makefile for Borland C++ 5.02

# Set compiler and linker
CC = bcc
# Compiler flags: -ml for large memory model, suitable for DOS
CFLAGS = -ml -w -O1

# Source files
SRCS = src/main.cpp src/filecopy.cpp src/progress.cpp src/logger.cpp
OBJS = src/main.obj src/filecopy.obj src/progress.obj src/logger.obj
EXEC = filecopy.exe

# Main build rule
$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) -e$(EXEC) $(OBJS)

# Rules for building .obj files from .cpp files
src/main.obj: src/main.cpp
	$(CC) $(CFLAGS) -c src/main.cpp -osrc/main.obj

src/filecopy.obj: src/filecopy.cpp
	$(CC) $(CFLAGS) -c src/filecopy.cpp -osrc/filecopy.obj

src/progress.obj: src/progress.cpp
	$(CC) $(CFLAGS) -c src/progress.cpp -osrc/progress.obj

src/logger.obj: src/logger.cpp
	$(CC) $(CFLAGS) -c src/logger.cpp -osrc/logger.obj

# Clean target
clean:
	del src\*.obj
	del $(EXEC)