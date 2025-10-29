/*
 * MIT License
 *
 * Copyright (c) 2022 Livox Tech. Company Limited
 * Copyright (c) 2025 Ibrahim Ismael
 *
 * This file has been modified from the original Livox ROS Driver2 source
 * refactored build, and multi-sensor support).
 */

#ifndef LIVOX_ROS_DRIVER_SEMAPHORE_H_
#define LIVOX_ROS_DRIVER_SEMAPHORE_H_

#include <mutex>
#include <condition_variable>

namespace livox_ros {

class Semaphore {
 public:
  explicit Semaphore(int count = 0) : count_(count) {
  }
  void Signal();
  void Wait();
  int GetCount() {
    return count_;
  }

 private:
  std::mutex mutex_;
  std::condition_variable cv_;
  volatile int count_;
};

} // namespace livox_ros

#endif // LIVOX_ROS_DRIVER_SEMAPHORE_H_
