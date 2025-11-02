#pragma once

#include <iostream>
#include <unistd.h>
#include <string>
#include <filesystem>
#include <fstream>
#include <ctime>
#include <memory>
#include <sstream>
#include "Mutex.hpp"

enum class LogLevel
{
  DEBUG,
  INFO,
  WARNING,
  ERROR,
  FATAL
};

std::string LogLeveltoString(LogLevel level)
{
  switch (level)
  {
  case LogLevel::DEBUG:
    return "DEBUG";
  case LogLevel::INFO:
    return "INFO";
  case LogLevel::WARNING:
    return "WARNING";
  case LogLevel::ERROR:
    return "ERROR";
  case LogLevel::FATAL:
    return "FATAL";
  default:
    return "UNKNOWN";
  }
}

// 获取日志时间
std::string GetCurrentTime()
{
  // 获取时间戳
  time_t currtime = time(nullptr);

  struct tm currtm;
  localtime_r(&currtime, &currtm);

  char timebuffer[64];
  snprintf(timebuffer, sizeof(timebuffer), "%4d-%02d-%02d %02d:%02d:%02d",
           currtm.tm_year + 1900,
           currtm.tm_mon + 1,
           currtm.tm_mday,
           currtm.tm_hour,
           currtm.tm_min,
           currtm.tm_sec);

  return timebuffer;
}

// 策略模式
class LogStrategy
{
public:
  virtual ~LogStrategy() = default;
  virtual void SyncLog(const std::string &message) = 0; // 不同模式核心的刷新模式不同
};

// 显示器刷新
class ConsoleLogStrategy : public LogStrategy
{
public:
  void SyncLog(const std::string &message) override
  {
    {
      LockGuard lockguard(&_mutex);
      std::cerr << message << std::endl;
    }
  }
  ~ConsoleLogStrategy()
  {
  }

private:
  Mutex _mutex;
};

const std::string logdefaultpath = "Logger";
const static std::string filedefaultname = "test.log";

// 文件刷新
class FileLogStrategy : public LogStrategy
{
public:
  FileLogStrategy(const std::string logpath = logdefaultpath, std::string logfilename = filedefaultname)
      : _logpath(logpath),
        _logfilename(logfilename)
  {
    if (std::filesystem::exists(_logpath))
      return;
    try
    {
      std::filesystem::create_directories(logpath);
    }
    catch (const std::filesystem::filesystem_error &e)
    {
      std::cerr << e.what() << "\r\n";
    }
  }

  void SyncLog(const std::string &message) override
  {
    {
      LockGuard lockguard(&_mutex);
      std::string target = _logpath;
      target += "/";
      target += _logfilename;

      std::ofstream out(target.c_str(), std::ios::app);
      if (!out.is_open())
      {
        return;
      }

      out << message << "\n"; // 相当于write文件操作
      out.close();
    }
  }

  ~FileLogStrategy()
  {
  }

private:
  std::string _logpath;
  std::string _logfilename;
  Mutex _mutex;
};

class Logger
{
public:
  Logger()
  {
  }

  void EnableConsoleLogStrategy()
  {
    _strategy = std::make_unique<ConsoleLogStrategy>();
  }
  void EnableFileLogStrategy()
  {
    _strategy = std::make_unique<FileLogStrategy>();
  }

  class LogMessage
  {
  public:
    LogMessage(LogLevel level, std::string &filename, int line, Logger &logger)
        : _curr_time(GetCurrentTime()),
          _level(level),
          _pid(getpid()),
          _filename(filename),
          _line(line),
          _logger(logger)
    {
      std::stringstream ss;
      ss << "[" << _curr_time << "]"
         << "[" << LogLeveltoString(_level) << "]"
         << "[" << _pid << "]"
         << "[" << _filename << "]"
         << "[" << _line << "]"
         << " - ";
      _loginfo = ss.str();
    }

    template <typename T>
    LogMessage &operator<<(const T &info)
    {
      std::stringstream ss;
      ss << info;
      _loginfo += ss.str();
      return *this;
    }

    ~LogMessage()
    {
      if (_logger._strategy)
      {
        _logger._strategy->SyncLog(_loginfo);
      }
    }

  private:
    std::string _curr_time;
    LogLevel _level;
    pid_t _pid;
    std::string _filename;
    int _line;

    std::string _loginfo; // 将日志信息汇总到一起
    Logger &_logger;
  };

  LogMessage operator()(LogLevel level, std::string filename, int line)
  {
    return LogMessage(level, filename, line, *this);
  }
  ~Logger()
  {
  }

private:
  std::unique_ptr<LogStrategy> _strategy;
};

Logger logger;
#define LOG(level) logger(level, __FILE__, __LINE__)
#define EnableConsoleLogStrategy() logger.EnableConsoleLogStrategy()
#define EnableFileLogStrategy() logger.EnableFileLogStrategy()