#pragma once

#if defined(_WIN32) || defined(_WIN64)
#include <source_location>
#else
#include <experimental/source_location>
#endif

#include <ctime>
#include <string>
#include <thread>

#include "filter.h"
namespace quark
{
#if defined(_WIN32) || defined(_WIN64)
typedef std::source_location source_location;
#else
typedef std::experimental::source_location source_location;
#endif
  class Record
  {
  public:
    Record(LogLevel level,
           std::string message,
           const source_location &location =
               source_location::current())
        : m_level(level),
          m_message(std::move(message)),
          m_tid(std::this_thread::get_id())
    {
      m_tm = time(nullptr);
      m_src = location.file_name();
      m_line = location.line();
      m_func = location.function_name();
    }

    // abstact to formatter
    std::string format()
    {
      std::ostringstream ss;
      ss << "[" << std::put_time(std::gmtime(&m_tm), "%c %Z") << "]";
      ss << "[" << LogLevelToString(m_level) << "]";
      ss << "[" << get_thread_id() << "]";
      ss << "[" << get_function() << "@" << m_line << "] ";
      ss << m_message; // << "\n";

      return ss.str();
    }

    const LogLevel &get_level() const { return m_level; }
    const std::time_t &get_time() const { return m_tm; }
    const std::thread::id &get_thread_id() const { return m_tid; }
    const char *get_source() const { return m_src; }
    const std::size_t get_line() const { return m_line; }
    const char *get_function() const { return m_func; }
    const std::string &get_message() const { return m_message; }

  private:
    LogLevel m_level;
    std::time_t m_tm;
    std::thread::id m_tid;
    const char *m_src;
    std::size_t m_line;
    const char *m_func;
    std::string m_message;
  };
} // namespace quark