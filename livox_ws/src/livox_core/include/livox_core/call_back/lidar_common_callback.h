/*
 * MIT License
 *
 * Copyright (c) 2022 Livox Tech. Company Limited
 * Copyright (c) 2025 Ibrahim Ismael
 *
 * This file has been modified from the original Livox ROS Driver2 source
 * refactored build, and multi-sensor support).
 */

#ifndef LIVOX_ROS_DRIVER_LIDAR_COMMON_CALLBACK_H_
#define LIVOX_ROS_DRIVER_LIDAR_COMMON_CALLBACK_H_	

#include "comm/comm.h"

namespace livox_ros {

class LidarCommonCallback {
 public:
  static void OnLidarPointClounCb(PointFrame* frame, void* client_data);
  static void LidarImuDataCallback(ImuData* imu_data, void *client_data);
};

} // namespace livox_ros

#endif // LIVOX_ROS_DRIVER_LIDAR_COMMON_CALLBACK_H_
