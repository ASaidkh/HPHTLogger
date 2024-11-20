/*
Author: Alim Saidkhodjaev
Created: 07/24/2024
Modified: 08/13/2024
Description: CSV appending operations
 */

#include "append_CSV.h"
#include <iostream>
#include <fstream>
#include <cerrno>
#include <cstring>
#include <thread>
#include <chrono>
#include <sys/stat.h>  // For file existence check
#include <vector>
#include <sstream>
#include <algorithm>

#define MAX_RETRIES 10
#define RETRY_DELAY_MS 50

// Function to check if the file exists
bool file_exists(const std::string& filename) {
    struct stat buffer;
    return (stat(filename.c_str(), &buffer) == 0);
}

// Function to try opening a file (for input)
bool try_open_file(std::ifstream& file, const std::string& filename, Logger& logger) {

    for (int attempt = 1; attempt <= MAX_RETRIES; attempt++) {
        file.open(filename);
        if (file.is_open()) {
            return true;
        }
        logger.log_error("Attempt " + std::to_string(attempt) + ": Error opening file: " + filename +
                         " Error: " + strerror(errno) + ". Retrying in " + std::to_string(RETRY_DELAY_MS) + "ms");
        std::this_thread::sleep_for(std::chrono::milliseconds(RETRY_DELAY_MS));
    }
    return false;
}

// Function to try opening a file (for output)
bool try_open_file(std::ofstream& file, const std::string& filename, std::ios_base::openmode mode, Logger& logger) {
    for (int attempt = 1; attempt <= MAX_RETRIES; attempt++) {
        file.open(filename, mode);
        if (file.is_open()) {
            return true;
        }
        logger.log_error("Attempt " + std::to_string(attempt) + ": Error opening file: " + filename +
                         " Error: " + strerror(errno) + ". Retrying in " + std::to_string(RETRY_DELAY_MS) + "ms");
        std::this_thread::sleep_for(std::chrono::milliseconds(RETRY_DELAY_MS));
    }
    return false;
}

// Function to read all lines from a file into a vector
std::vector<std::string> read_lines(const std::string& filename) {
    std::ifstream file(filename);
    std::vector<std::string> lines;
    std::string line;

    while (std::getline(file, line)) {
        lines.push_back(line);
    }
    return lines;
}

// Function to append data to the destination CSV file in order
void append_csv(const std::string& source_file, const std::string& destination_file, Logger& logger) {
    logger.log("Opening source file: " + source_file);
    std::ifstream src;
    if (!try_open_file(src, source_file, logger)) {
        logger.log_error("Failed to open source file after " + std::to_string(MAX_RETRIES) + " attempts: " + source_file);
        return;
    }
    logger.log("Successfully opened source file: " + source_file);

    std::string header_line;
    if (!std::getline(src, header_line)) {
        logger.log_error("Error reading header line from source file: " + source_file);
        src.close();
        return;
    }
    logger.log("Read header line from source file: " + header_line);

    if (!file_exists(destination_file)) {
        // Create the file if it doesn't exist
        logger.log("File does not exist, creating file: " + destination_file);
        std::ofstream tmp(destination_file);
        tmp.close(); // Close the file immediately after creating it
    }

    std::ifstream dest_read;
    bool header_exists = false;
   
    if (try_open_file(dest_read, destination_file, logger)) {
        std::string dest_first_line;
        if (std::getline(dest_read, dest_first_line)) {
            if (dest_first_line == header_line) {
                header_exists = true;
                logger.log("Header already exists in destination file.");
            }
        }
        dest_read.close();  // Close the destination read stream
    }

    std::vector<std::string> dest_lines = read_lines(destination_file);
    std::ofstream dest;
    if (!try_open_file(dest, destination_file, std::ios::app, logger)) {
        logger.log_error("Failed to open destination file after " + std::to_string(MAX_RETRIES) + " attempts: " + destination_file);
        src.close();
        return;
    }
    logger.log("Successfully opened destination file: " + destination_file);

    // Append header if it does not exist
    if (!header_exists) {
        dest << header_line << "\n";
        logger.log("Appended header to destination file: " + header_line);
    }

    std::string first_src_line;

    // Process the source lines and insert them in order
    while (std::getline(src, first_src_line)) {
        std::stringstream ss(first_src_line);
        std::string src_timestamp;
        std::getline(ss, src_timestamp, ','); // Assuming the timestamp is the first column

        // Get the last timestamp from the destination
        std::string last_dest_timestamp;
        if (!dest_lines.empty()) {
            std::stringstream last_line_stream(dest_lines.back());
            std::getline(last_line_stream, last_dest_timestamp, ',');
        }

        // Check if the source timestamp is in order
        if (!last_dest_timestamp.empty() && src_timestamp <= last_dest_timestamp) {
            // Find the position to insert the new data
            auto it = std::lower_bound(dest_lines.begin(), dest_lines.end(), first_src_line);
            size_t position = std::distance(dest_lines.begin(), it);

            // Shift lines down
            for (size_t i = dest_lines.size(); i > position; --i) {
                dest << dest_lines[i - 1] << "\n"; // Write lines in reverse order
            }
            dest << first_src_line << "\n"; // Write the new line
        } else {
            // If in order, append directly
            dest << first_src_line << "\n";
        }
    }

    // Close files after use
    src.close();
    dest.close();
    logger.log("Data appended successfully and files closed!");
}

// Function to copy a CSV file to a destination with retries
void copy_CSV(const std::string& source_file, const std::string& destination_file, Logger& logger) {
    logger.log("Starting copy operation from " + source_file + " to " + destination_file);

    std::ifstream src;
    if (!try_open_file(src, source_file, logger)) {
        logger.log_error("Failed to open source file after " + std::to_string(MAX_RETRIES) + " attempts: " + source_file);
        return;
    }
    logger.log("Successfully opened source file: " + source_file);

    std::ofstream dest;
    if (!try_open_file(dest, destination_file, std::ios::trunc, logger)) {  // Use trunc to overwrite the destination file
        logger.log_error("Failed to open destination file after " + std::to_string(MAX_RETRIES) + " attempts: " + destination_file);
        src.close();
        return;
    }
    logger.log("Successfully opened destination file: " + destination_file);

    // Copy the content
    dest << src.rdbuf();

    // Close files after use
    src.close();
    dest.close();
    logger.log("File copy operation completed successfully and files closed!");
}