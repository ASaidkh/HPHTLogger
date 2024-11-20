#ifndef FILEPARSER_H
#define FILEPARSER_H

#include <string>
#include "Logger.h"

// Function to get the last line of a file
std::string getLastLine(const std::string& filename, Logger& logger);

// Function to extract a field from a log line
std::string extractField(const std::string& logLine, int fieldNum, Logger& logger);

// Function to return the filename from a file path
std::string getFilename(const std::string& path);

#endif // FILEPARSER_H
