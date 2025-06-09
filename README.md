# DOS File Copy Utility

This project is a console application designed to copy files in the MSDOS 6.22 environment. It provides a user-friendly interface for specifying source and destination file paths, and it includes features such as a progress indicator and transfer speed monitoring.

## Features

- **File Copying**: Efficiently copies files from a specified source to a destination.
- **Progress Indicator**: Displays a progress bar in the console to indicate the status of the file transfer.
- **Speed Monitoring**: Shows the current speed of the transfer in real-time.
- **Logging**: Creates a log file in the destination folder with details about the transfer, including:
  - Date and time of the transfer
  - File size
  - Source file path
  - Maximum, minimum, and average speed
  - Total duration of the transfer

## Project Structure

```
dos-file-copy
├── src
│   ├── main.cpp        # Entry point of the application
│   ├── filecopy.cpp    # Implementation of file copying functionality
│   ├── filecopy.h      # Header file for FileCopy class
│   ├── progress.cpp     # Implementation of progress bar functionality
│   ├── progress.h       # Header file for Progress class
│   ├── logger.cpp       # Implementation of logging functionality
│   ├── logger.h         # Header file for Logger class
│   └── utils.h          # Utility functions and constants
├── makefile             # Makefile for building the project
└── README.md            # Documentation for the project
```

## Compilation and Execution

To compile the project, navigate to the project directory and run the following command:

```
make
```

This will generate the executable file. To run the application, use the following command:

```
FILECOPY.EXE <source_file_path> <destination_file_path>
```

Replace `<source_file_path>` with the full path of the file you want to copy and `<destination_file_path>` with the desired destination path.

## Usage Example

```
FILECOPY.EXE D:\folder\file.ext D:\folder\copy_of_file.ext
```

This command will copy `file.ext` from `D:\folder` to `D:\folder\copy_of_file.ext`, displaying the progress and logging the transfer details.

## License

This project is licensed under the MIT License. See the LICENSE file for more details.
