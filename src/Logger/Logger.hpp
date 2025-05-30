/**
 * @file Logger.hpp
 * @brief Defines the Logger class for logging messages.
 */

#pragma once

#include <fstream>
#include <mutex>
#include <string>

namespace Plazza::Logger {
/**
 * @enum LogLevel
 * @brief Enum representing different log levels.
 */
enum class LogLevel { DEBUG = 0, INFO = 1, WARN = 2, ERROR = 3, FATAL = 4 };

/**
 * @class Logger
 * @brief Singleton class for logging messages with different severity levels.
 */
class Logger {
public:
  /**
   * @brief Gets the singleton instance of the Logger.
   * @return Reference to the Logger instance.
   */
  static Logger &getInstance();

  /**
   * @brief Sets the log level for the Logger.
   * @param level The log level to set.
   */
  void setLogLevel(LogLevel level);

  /**
   * @brief Enables or disables logging to a file.
   * @param enable Whether to enable file logging.
   * @param filename The name of the log file (default is "plazza.log").
   */
  void setLogToFile(bool enable, const std::string &filename = "plazza.log");

  /**
   * @brief Logs a message with the specified log level.
   * @param level The log level of the message.
   * @param message The message to log.
   */
  void log(LogLevel level, const std::string &message);

  /**
   * @brief Logs a debug message.
   * @param message The debug message to log.
   */
  void debug(const std::string &message);

  /**
   * @brief Logs an informational message.
   * @param message The informational message to log.
   */
  void info(const std::string &message);

  /**
   * @brief Logs a warning message.
   * @param message The warning message to log.
   */
  void warn(const std::string &message);

  /**
   * @brief Logs an error message.
   * @param message The error message to log.
   */
  void error(const std::string &message);

  /**
   * @brief Logs a fatal error message and terminates the program.
   * @param message The fatal error message to log.
   */
  [[noreturn]] void fatal(const std::string &message);

private:
  /**
   * @brief Constructor for the Logger.
   */
  Logger() = default;

  /**
   * @brief Destructor for the Logger.
   */
  ~Logger() = default;

  Logger(const Logger &) = delete;
  Logger &operator=(const Logger &) = delete;

  /**
   * @brief Formats a log message with the current timestamp and log level.
   * @param level The log level of the message.
   * @param message The message to format.
   * @return The formatted log message.
   */
  std::string formatMessage(LogLevel level, const std::string &message) const;

  /**
   * @brief Gets the current timestamp as a string.
   * @return The current timestamp in "YYYY-MM-DD HH:MM:SS" format.
   */
  std::string getCurrentTimestamp() const;

  /**
   * @brief Converts a log level to its string representation.
   * @param level The log level to convert.
   * @return The string representation of the log level.
   */
  std::string logLevelToString(LogLevel level) const;

  /**
   * @brief Gets the color code for console output based on the log level.
   * @param level The log level to get the color code for.
   * @return The ANSI color code as a string.
   */
  std::string getColorCode(LogLevel level) const;

private:
  LogLevel m_logLevel{LogLevel::INFO};
  bool m_logToFile{false};
  std::string m_logFilename;
  std::ofstream m_logFile;
  mutable std::mutex m_mutex;
};

} // namespace Plazza::Logger

#define LOG_DEBUG(msg) Plazza::Logger::Logger::getInstance().debug(msg)
#define LOG_INFO(msg) Plazza::Logger::Logger::getInstance().info(msg)
#define LOG_WARN(msg) Plazza::Logger::Logger::getInstance().warn(msg)
#define LOG_ERROR(msg) Plazza::Logger::Logger::getInstance().error(msg)
#define LOG_FATAL(msg) Plazza::Logger::Logger::getInstance().fatal(msg)
