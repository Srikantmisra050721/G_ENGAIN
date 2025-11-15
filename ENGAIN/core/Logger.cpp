#include "Logger.h"
#include <filesystem>

namespace ENGAIN {

Logger::Logger() : currentLevel(LogLevel::DEBUG), initialized(false) {}

Logger::~Logger() {
    if (logFile.is_open()) {
        info("Logger shutting down");
        logFile.close();
    }
}

Logger& Logger::getInstance() {
    static Logger instance;
    return instance;
}

void Logger::initialize(const std::string& logDir) {
    if (initialized) return;
    
    // Create logs directory if it doesn't exist
    std::filesystem::create_directories(logDir);
    
    // Create log filename with timestamp
    std::time_t now = std::time(nullptr);
    std::tm* localTime = std::localtime(&now);
    
    std::ostringstream filename;
    filename << logDir << "/engain_"
             << std::put_time(localTime, "%Y%m%d_%H%M%S")
             << ".log";
    
    logFile.open(filename.str(), std::ios::out | std::ios::app);
    
    if (logFile.is_open()) {
        initialized = true;
        info("Logger initialized");
    } else {
        std::cerr << "Failed to open log file: " << filename.str() << std::endl;
    }
}

void Logger::setLevel(LogLevel level) {
    currentLevel = level;
}

void Logger::debug(const std::string& message) {
    log(LogLevel::DEBUG, message);
}

void Logger::info(const std::string& message) {
    log(LogLevel::INFO, message);
}

void Logger::warning(const std::string& message) {
    log(LogLevel::WARNING, message);
}

void Logger::error(const std::string& message) {
    log(LogLevel::ERROR, message);
}

void Logger::critical(const std::string& message) {
    log(LogLevel::CRITICAL, message);
}

void Logger::log(LogLevel level, const std::string& message) {
    if (level < currentLevel) return;
    
    std::string timestamp = getCurrentTime();
    std::string levelStr = levelToString(level);
    
    std::ostringstream logMessage;
    logMessage << "[" << timestamp << "] [" << levelStr << "] " << message;
    
    // Write to console
    if (level >= LogLevel::INFO) {
        std::cout << logMessage.str() << std::endl;
    }
    
    // Write to file
    if (initialized && logFile.is_open()) {
        logFile << logMessage.str() << std::endl;
        logFile.flush();
    }
}

std::string Logger::getCurrentTime() {
    std::time_t now = std::time(nullptr);
    std::tm* localTime = std::localtime(&now);
    
    std::ostringstream oss;
    oss << std::put_time(localTime, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

std::string Logger::levelToString(LogLevel level) {
    switch (level) {
        case LogLevel::DEBUG:    return "DEBUG";
        case LogLevel::INFO:     return "INFO";
        case LogLevel::WARNING:  return "WARNING";
        case LogLevel::ERROR:    return "ERROR";
        case LogLevel::CRITICAL: return "CRITICAL";
        default:                 return "UNKNOWN";
    }
}

} // namespace ENGAIN
