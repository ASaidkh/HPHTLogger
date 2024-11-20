/*
Author: Alim Saidkhodjaev
Creaed: 08/13/2024
Description: File parsing operations
 */
#include "FileParser.h"
#include <fstream>
#include <sstream>
#include <vector>

// Function to get the last line of a file
std::string getLastLine(const std::string& filename, Logger& logger) {
    std::ifstream file(filename, std::ios::in);
    if (!file.is_open()) {
        logger.log_error("Unable to open file: " + filename);
        return "Unable to open file";
    }
    std::string line;
    std::string lastLine;
    while (std::getline(file, line)) {
        lastLine = line;
    }
    file.close();
    return lastLine;
}

// Function to extract a field from a log line
std::string extractField(const std::string& logLine, int fieldNum, Logger& logger) {
    std::istringstream iss(logLine);
    std::string token;
    std::vector<std::string> tokens;

    while (iss >> token) {
        tokens.push_back(token);
    }

    // Extract desired field based on fieldNum
    if (tokens.size() > fieldNum - 1) {
        std::string filePath = tokens[fieldNum - 1];
        // Find the position of ".tmp"
        size_t pos = filePath.find(".tmp");
        if (pos != std::string::npos) {
            // Remove the ".tmp" part and everything after it
            filePath = filePath.substr(0, pos);
        }
        return filePath;
    } else {
        logger.log_error("Unable to extract file path from log line: " + logLine);
        return "Unable to extract file path";
    }
}

// Function to return the filename from a file path
std::string getFilename(const std::string& path) {
    size_t pos = path.find_last_of("/\\");
    return (pos != std::string::npos) ? path.substr(pos + 1) : path;
}
