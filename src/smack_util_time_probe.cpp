/* Smack C++ @ https://github.com/smacklib/dev_smack_cpp
 *
 * Time measurement support.
 * 
 * Copyright © 2019 Michael Binz
 */

#include <iostream>

#include "smack_util_time_probe.hpp"

namespace smack {
namespace util {

void TimeProbe::reset() {
  a_start =
    std::chrono::high_resolution_clock::now();
}

TimeProbe::TimeProbe(std::string message)
  : a_message(message) {
  reset();
}

TimeProbe::TimeProbe(const char* message)
  : a_message(message) {
  reset();
}

double TimeProbe::duration() {
  std::chrono::high_resolution_clock::time_point now =
    std::chrono::high_resolution_clock::now();

  std::chrono::duration<double> time_span =
    std::chrono::duration_cast<std::chrono::duration<double>>(now - a_start);

  return time_span.count();
}

void TimeProbe::toString() {
  std::cout << a_message << ": " << duration() << "sec" << std::endl;
}

} // namepace util
} // namespace smack
