#include "logger.hpp"

FileLogger::FileLogger(const std::string& filename) {
    fstr.open(filename);
    // file_ptr = fopen(filename.c_str(), "w");
    // if (file_ptr == nullptr) {
    //     throw std::bad_alloc();  // technically an allocation
    // }
}

FileLogger::~FileLogger() { fstr.close(); }

void FileLogger::info(const std::string& message) {
    // fprintf(file_ptr, "%s\n", message.c_str());
    fstr << "INFO: " << message << std::endl;
}

void FileLogger::warning(const std::string& message) {
    // fprintf(file_ptr, "%s\n", message.c_str());
    fstr << "WARNING: " <<message << std::endl;
}

void FileLogger::error(const std::string& message) {
    // fprintf(file_ptr, "%s\n", message.c_str());
    fstr << "ERROR: " << message << std::endl;
}

void StderrLogger::info(const std::string& message) {
    std::cerr << "INFO: " << message << std::endl;
}

void StderrLogger::warning(const std::string& message) {
    std::cerr << "WARNING: " << message << std::endl;
}

void StderrLogger::error(const std::string& message) {
    std::cerr << "ERROR: " << message << std::endl;
}

void CompositeLogger::info(const std::string& message) {
    for (Logger* cur_ptr : loggers) {
        if (cur_ptr == nullptr)
            throw std::invalid_argument(
                "nullpointer occured");
        cur_ptr->info(message);
    }
}

void CompositeLogger::warning(const std::string& message) {
    for (Logger* cur_ptr : loggers) {
        if (cur_ptr == nullptr)
            throw std::invalid_argument(
                "nullpointer occured");
        cur_ptr->warning(message);
    }
}

void CompositeLogger::error(const std::string& message) {
    for (Logger* cur_ptr : loggers) {
        if (cur_ptr == nullptr)
            throw std::invalid_argument(
                "nullpointer occured");
        cur_ptr->error(message);
    }
}

void CompositeLogger::add(Logger* logger) { loggers.push_back(logger); }