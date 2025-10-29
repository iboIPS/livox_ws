/*
 * MIT License
 *
 * Copyright (c) 2022 Livox Tech. Company Limited
 * Copyright (c) 2025 Ibrahim Ismael
 *
 * This file has been modified from the original Livox ROS Driver2 source
 * refactored build, and multi-sensor support).
 */

#ifndef LIVOX_ROS_DRIVER_PARSE_CFG_FILE_H_
#define LIVOX_ROS_DRIVER_PARSE_CFG_FILE_H_

#include "../comm/comm.h"

#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/stringbuffer.h"

#include <string>
#include <vector>

namespace livox_ros {

class ParseCfgFile {
 public:
  explicit ParseCfgFile(const std::string& path);
  ~ParseCfgFile() {}

  bool ParseSummaryInfo(LidarSummaryInfo& lidar_summary_info);
  
 private:
  const std::string path_;
};

} // namespace livox_ros

#endif // LIVOX_ROS_DRIVER_PARSE_CFG_FILE_H_
