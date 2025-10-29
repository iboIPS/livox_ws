/*
 * MIT License
 *
 * Copyright (c) 2022 Livox Tech. Company Limited
 * Copyright (c) 2025 Ibrahim Ismael
 *
 * This file has been modified from the original Livox ROS Driver2 source
 * refactored build, and multi-sensor support).
*/

#ifndef LIVOX_DRIVER_NODE_H
#define LIVOX_DRIVER_NODE_H

#include "ros_headers.h"

namespace livox_ros {

class Lddc;

class DriverNode final : public rclcpp::Node {
 public:
  explicit DriverNode(const rclcpp::NodeOptions& options);
  DriverNode(const DriverNode &) = delete;
  ~DriverNode();
  DriverNode &operator=(const DriverNode &) = delete;

  DriverNode& GetNode() noexcept;

 private:
  void PointCloudDataPollThread();
  void ImuDataPollThread();
  void OnAfterConstruction();
  double GetPublishFreqFromConfig(const std::string &config_path, rclcpp::Logger logger);

  std::unique_ptr<Lddc> lddc_ptr_;
  std::shared_ptr<std::thread> pointclouddata_poll_thread_;
  std::shared_ptr<std::thread> imudata_poll_thread_;
  std::shared_future<void> future_;
  std::promise<void> exit_signal_;

  rclcpp::TimerBase::SharedPtr post_init_timer_;

};

} // namespace livox_ros

#endif // LIVOX_DRIVER_NODE_H