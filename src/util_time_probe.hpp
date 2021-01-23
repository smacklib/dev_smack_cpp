/*
 * Smack C++
 *
 * Copyright © 2019 Michael Binz
 */

#pragma once
#include <string>
#include <chrono>
#include <thread>
#include <vector>
#include <algorithm>
#include <cmath>
#include <numeric>

namespace smack {
namespace util {

/**
 * Allows in-program time measurements. Internally uses hi-res timers.
 */
class TimeProbe {
  std::string a_message;
  std::chrono::high_resolution_clock::time_point a_start;

public:
  /**
   * Creates an instance.
   *
   * @param message A message used when printing the timeprobe's value.
   */
  TimeProbe(std::string message);

  /**
   * Creates an instance.
   *
   * @param message A message used when printing the timeprobe's value.
   */
  TimeProbe(const char* message);

  /**
   * Reset the time counter, starts with zero.
   */
  void reset();

  /**
   * @return The number of seconds since creaton or last reset.
   */
  double duration();

  /**
   * Dump to stdout.
   */
  void toString();

  static void sleepMs(uint32_t ms) {
    std::this_thread::sleep_for(
      std::chrono::milliseconds(ms));
  }

  /**
   * Performs times executions of the passed lambda.  Records the times
   * and returns the minimum time in seconds.
   * @param times The number of executions to be measured.
   * @param lambda The lambda to execute.
   * @param percent How many of the probe results to use. 0 and smaller
   *        uses only the single best value. Values between 0.0 and 1.0
   *        return the average of the best 'percent' measurements.
   *        Values 1.0 and greater return the average of all probes.
   * @return The minimum execution time in seconds.
   */
  template <typename L>
  static double profile(
    unsigned int times,
    L lambda,
    double percent = 0.05) {
    if (!times)
      return 0.0;

    unsigned int averageCount;

    if (percent <= 0)
      averageCount = 1;
    else if (percent > 1.0)
      averageCount = times;
    else
      averageCount =
      (unsigned int)rint(times * percent);

    if (averageCount < 1)
      averageCount = 1;

    std::vector<double> probes;
    probes.reserve(times);

    TimeProbe tp("profile");

    for (unsigned int i = 0; i < times; i++) {
      tp.reset();
      lambda();
      probes.push_back(tp.duration());
    }

    std::sort(probes.begin(), probes.end());

    return
      std::accumulate(probes.begin(), probes.begin() + averageCount, 0.0)
      /
      averageCount;
  }

  /**
   * Profiles the passed lambda.
   *
   * @param lambda The lambda to execute.
   * @return The execution time in seconds.
   */
  template <typename L>
  static double profile(
    L lambda) {
    TimeProbe tp("profile");

    lambda();

    return tp.duration();
  }
};

} // namespace util
} // namespace smack
