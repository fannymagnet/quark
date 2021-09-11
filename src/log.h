#ifndef _QUARK_LOG_H_
#define _QUARK_LOG_H_

#include "log/file.h"
#include "log/record.h"

//#include <experimental/source_location>
#include <string>
#include <iostream>

namespace quark
{

  /*
  std::cout << "testing log" << std::endl;
  const std::experimental::source_location& location =
      std::experimental::source_location::current();
  auto source1 = location.file_name();
  std::cout << "info:" << location.file_name() << ":" << location.line() << " "
            << "xxxxxx" << '\n';

  const std::experimental::source_location& location2 =
      std::experimental::source_location::current();
  auto source2 = location2.file_name();
  std::cout << "s1: " << source1 << "address:" << &source1 << "/n"
            << "s2: " << source2 << "address:" << &source2 << std::endl;

  Record r(debug, "test log record");
  std::cout << r.format();

  auto f = File("t.log");
  if (f.open()) {
    std::string s = r.format();

    f.write(s.c_str(), s.size());
  }
*/
#define LOCATION source_location::current()

  template <typename T>
  static void Log(T arg)
  {
    std::cout << arg;
  }

  template <typename T, typename... Ts>
  static void Log(T arg1, Ts... arg_left)
  {
    std::cout << arg1;
    Log(arg_left...);
  }

  static void Debug(std::string log, const source_location &location =
                                  source_location::current())
  {
    Record r(debug, log, location);
    std::cout << r.format();
    std::cout << std::endl;
  }

  template <typename... Ts>
  static void Debug(const source_location &location, std::string log, Ts... args)
  {
    Record r(debug, log, location);
    std::cout << r.format();
    Log(args...);
    std::cout << std::endl;
  }
}
#endif //_QUARK_LOG_H_