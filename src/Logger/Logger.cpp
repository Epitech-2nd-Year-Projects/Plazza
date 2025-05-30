#include "Logger/Logger.hpp"
#include <filesystem>
#include <iostream>
#include <thread>

namespace Plazza::Logger {

Logger &Logger::getInstance() {
  static Logger instance;
  return instance;
}

void Logger::setLogLevel(LogLevel level) {
  std::lock_guard<std::mutex> lock(m_mutex);
  m_logLevel = level;
}

void Logger::setLogToFile(bool enable, const std::string &filename) {
  std::lock_guard<std::mutex> lock(m_mutex);

  if (m_logFile.is_open()) {
    m_logFile.close();
  }

  m_logToFile = enable;
  if (enable) {
    m_logFilename = filename;

    std::filesystem::path filePath(filename);
    if (filePath.has_parent_path()) {
      std::filesystem::create_directories(filePath.parent_path());
    }

    m_logFile.open(filename, std::ios::app);
    if (!m_logFile.is_open()) {
      std::cerr << "Failed to open log file: " << filename << std::endl;
      m_logToFile = false;
    }
  }
}

void Logger::log(LogLevel level, const std::string &message) {
  std::lock_guard<std::mutex> lock(m_mutex);

  if (level < m_logLevel) {
    return;
  }

  std::string formattedMessage = formatMessage(level, message);

  if (level >= LogLevel::ERROR) {
    std::cerr << formattedMessage << std::endl;
  } else {
    std::cout << formattedMessage << std::endl;
  }

  if (m_logToFile && m_logFile.is_open()) {
    std::string fileMessage = formattedMessage;
    std::size_t position = 0;
    while ((position = fileMessage.find("\033[", position)) !=
           std::string::npos) {
      std::size_t end = fileMessage.find("m", position);
      if (end != std::string::npos) {
        fileMessage.erase(position, end - position + 1);
      } else {
        break;
      }
    }
    m_logFile << fileMessage << std::endl;
    m_logFile.flush();
  }
}

void Logger::debug(const std::string &message) {
  log(LogLevel::DEBUG, message);
}

void Logger::info(const std::string &message) { log(LogLevel::INFO, message); }

void Logger::warn(const std::string &message) { log(LogLevel::WARN, message); }

void Logger::error(const std::string &message) {
  log(LogLevel::ERROR, message);
}

void Logger::fatal(const std::string &message) {
  log(LogLevel::FATAL, message);
  std::exit(1);
}

std::string Logger::formatMessage(LogLevel level,
                                  const std::string &message) const {
  std::ostringstream oss;

  oss << getColorCode(level);
  oss << "[" << getCurrentTimestamp() << "] ";
  oss << "[" << logLevelToString(level) << "] ";
  oss << "[T-ID:" << std::this_thread::get_id() << "] ";
  oss << message;
  oss << "\033[0m";

  return oss.str();
}

std::string Logger::getCurrentTimestamp() const {
  auto now = std::chrono::system_clock::now();
  std::time_t timestamp = std::chrono::system_clock::to_time_t(now);

  std::ostringstream oss;
  oss << std::put_time(std::localtime(&timestamp), "%Y-%m-%d %H:%M:%S");
  return oss.str();
}

std::string Logger::logLevelToString(LogLevel level) const {
  switch (level) {
  case LogLevel::DEBUG:
    return "DEBUG";
  case LogLevel::INFO:
    return "INFO";
  case LogLevel::WARN:
    return "WARN";
  case LogLevel::ERROR:
    return "ERROR";
  case LogLevel::FATAL:
    return "FATAL";
  default:
    return "UNKNOWN";
  }
}

std::string Logger::getColorCode(LogLevel level) const {
  switch (level) {
  case LogLevel::DEBUG:
    return "\033[36m";
  case LogLevel::INFO:
    return "\033[32m";
  case LogLevel::WARN:
    return "\033[33m";
  case LogLevel::ERROR:
    return "\033[31m";
  case LogLevel::FATAL:
    return "\033[35m";
  default:
    return "\033[0m";
  }
}

} // namespace Plazza::Logger
