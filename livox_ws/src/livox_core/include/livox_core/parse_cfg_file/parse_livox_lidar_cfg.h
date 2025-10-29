/*
 * MIT License
 *
 * Copyright (c) 2022 Livox Tech. Company Limited
 * Copyright (c) 2025 Ibrahim Ismael
 *
 * This file has been modified from the original Livox ROS Driver2 source
 * refactored build, and multi-sensor support).
 */

#ifndef LIVOX_ROS_DRIVER_LIVOX_LIDAR_CFG_PARSER_H_
#define LIVOX_ROS_DRIVER_LIVOX_LIDAR_CFG_PARSER_H_

#include "comm/comm.h"
#include "livox_lidar_def.h"

#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/stringbuffer.h"

#include <iostream>
#include <string>
#include <vector>

namespace livox_ros {
  
class LivoxLidarConfigParser {
 public:
  explicit LivoxLidarConfigParser(const std::string& path)  : path_(path) {}
  ~LivoxLidarConfigParser() {}

  bool Parse(std::vector<UserLivoxLidarConfig> &lidar_configs);

 private:
  bool ParseUserConfigs(const rapidjson::Document &doc,
                         std::vector<UserLivoxLidarConfig> &user_configs);
  bool ParseExtrinsics(const rapidjson::Value &value, ExtParameter &param);

  const std::string path_;
};

} // namespace livox_ros

#endif // LIVOX_ROS_DRIVER_LIVOX_LIDAR_CFG_PARSER_H_
