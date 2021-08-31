#pragma once

namespace quark
{

  enum LogLevel
  {
    none = 0,
    error = 1,
    warning = 2,
    info = 3,
    debug = 4,
    trace = 5
  };

  inline const char *LogLevelToString(LogLevel level)
  {
    switch (level)
    {
    case error:
      return "ERROR";
    case warning:
      return "WARN";
    case info:
      return "INFO";
    case debug:
      return "DEBUG";
    case trace:
      return "TRACE";
    default:
      return "NONE";
    }
  }

  inline LogLevel LogLevelFromString(const char *str)
  {
    switch (std::toupper(str[0]))
    {
    case 'E':
      return error;
    case 'W':
      return warning;
    case 'I':
      return info;
    case 'D':
      return debug;
    case 'T':
      return trace;
    default:
      return none;
    }
  }
} // namespace quark