/*
 * MIT License
 *
 * Copyright (c) 2022 Livox Tech. Company Limited
 * Copyright (c) 2025 Ibrahim Ismael
 *
 * This file has been modified from the original Livox ROS Driver2 source
 * refactored build, and multi-sensor support).
 */

#include "livox_node/driver_node.h"
#include "livox_node/lddc.h"

namespace livox_ros {

DriverNode& DriverNode::GetNode() noexcept {
  return *this;
}

DriverNode::~DriverNode() {
  lddc_ptr_->lds_->RequestExit();
  exit_signal_.set_value();
  pointclouddata_poll_thread_->join();
  imudata_poll_thread_->join();
}

} // namespace livox_ros





