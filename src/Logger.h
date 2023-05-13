#pragma once
#include "Eigen/Core"

#include "spdlog/spdlog.h"

#include <memory>

class Logger
{
public:
	static bool Initialize();
	static void Shutdown();

  template<typename ... Args>
  static void Trace(const char* fmt, Args ... args)
  {
    if (consoleLogger != nullptr)
    {
      std::unique_ptr<char[]> formattedString = Format(fmt, args...);
      consoleLogger->trace(formattedString.get());
    }
  }

  template<typename ... Args>
  static void Info(const char* fmt, Args ... args)
  {
    if (consoleLogger != nullptr)
    {
      std::unique_ptr<char[]> formattedString = Format(fmt, args...);
      consoleLogger->info(formattedString.get());
    }
  }

  template<typename ... Args>
  static void Debug(const char* fmt, Args ... args)
  {
    if (consoleLogger != nullptr)
    {
      std::unique_ptr<char[]> formattedString = Format(fmt, args...);
      consoleLogger->debug(formattedString.get());
    }
  }

  template<typename ... Args>
  static void Error(const char* fmt, Args ... args)
  {
    if (consoleLogger != nullptr)
    {
      std::unique_ptr<char[]> formattedString = Format(fmt, args...);
      consoleLogger->error(formattedString.get());
    }
  }

  template<typename ... Args>
  static void Warn(const char* fmt, Args ... args)
  {
    if (consoleLogger != nullptr)
    {
      std::unique_ptr<char[]> formattedString = Format(fmt, args...);
      consoleLogger->warn(formattedString.get());
    }
  }

  static void TraceVector(Eigen::Vector3f& v, const char* label)
  {
    Trace("%s: (%.3f, %.3f, %.3f)", label, v.x(), v.y(), v.z()); 
  }

  static void TraceVector(Eigen::Vector3f& v)
  {
    Trace("(%.3f, %.3f, %.3f)", v.x(), v.y(), v.z()); 
  }

  static void DebugVector(Eigen::Vector3f v, const char* label)
  {
    Debug("%s: (%.3f, %.3f, %.3f)", label, v.x(), v.y(), v.z()); 
  }

  static void DebugVector(Eigen::Vector3f v)
  {
    Debug("(%.3f, %.3f, %.3f)", v.x(), v.y(), v.z()); 
  }

private:
    template<typename ... Args>
    static std::unique_ptr<char[]> Format(const char* fmt, Args ... args)
    {
      size_t size = snprintf(nullptr, 0, fmt, args...); 
      if (size <= 0)
        Error("Error while formatting.");

      std::unique_ptr<char[]> buffer = std::make_unique<char[]>(size + 1);
      snprintf(buffer.get(), size + 1, fmt, args...);

      return buffer;
    }

private:
    static std::shared_ptr<spdlog::logger> consoleLogger;
};

// Depricated macros
#define LOGGER_NAME	"ConsoleLogger"
#define LOG_TRACE(...)	if (spdlog::get(LOGGER_NAME) != nullptr) { spdlog::get(LOGGER_NAME)->trace(__VA_ARGS__); }
#define LOG_INFO(...)		if (spdlog::get(LOGGER_NAME) != nullptr) { spdlog::get(LOGGER_NAME)->info(__VA_ARGS__); }
#define LOG_DEBUG(...)	if (spdlog::get(LOGGER_NAME) != nullptr) { spdlog::get(LOGGER_NAME)->debug(__VA_ARGS__); }
#define LOG_ERROR(...)	if (spdlog::get(LOGGER_NAME) != nullptr) { spdlog::get(LOGGER_NAME)->error(__VA_ARGS__); }
#define LOG_WARN(...)	  if (spdlog::get(LOGGER_NAME) != nullptr) { spdlog::get(LOGGER_NAME)->warn(__VA_ARGS__); }
