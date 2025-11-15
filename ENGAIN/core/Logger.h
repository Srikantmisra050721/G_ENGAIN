#pragma once

#include <string>
#include <fstream>
#include <iostream>
#include <memory>
#include <ctime>
#include <iomanip>
#include <sstream>

namespace ENGAIN {

enum class LogLevel {
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    CRITICAL
};

class Logger {
public:
    static Logger& getInstance();
    
    void initialize(const std::string& logDir = "logs");
    void setLevel(LogLevel level);
    
    void debug(const std::string& message);
    void info(const std::string& message);
    void warning(const std::string& message);
    void error(const std::string& message);
    void critical(const std::string& message);
    
    ~Logger();

private:
    Logger();
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    
    void log(LogLevel level, const std::string& message);
    std::string getCurrentTime();
    std::string levelToString(LogLevel level);
    
    LogLevel currentLevel;
    std::ofstream logFile;
    bool initialized;
};

} // namespace ENGAIN
