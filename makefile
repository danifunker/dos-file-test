# Makefile for Borland C++ 5.02

# Set compiler and linker
CC = bcc
# Compiler flags: -ml for large memory model, suitable for DOS
CFLAGS = -ml -w -O1

# Source files
SRCS = src/main.cpp src/filecopy.cpp src/progress.cpp src/logger.cpp
OBJS = $(SRCS:.cpp=.obj)
EXEC = filecopy.exe

# Main build rule
$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) -e$(EXEC) $(OBJS)

# Rule for building .obj files from .cpp files
.cpp.obj:
	$(CC) $(CFLAGS) -c $< -o$@

# Clean target
clean:
	del *.obj
	del $(EXEC)