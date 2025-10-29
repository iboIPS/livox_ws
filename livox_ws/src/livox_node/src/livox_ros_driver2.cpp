/*
 * MIT License
 *
 * Copyright (c) 2022 Livox Tech. Company Limited
 * Copyright (c) 2025 Ibrahim Ismael
 *
 * This file has been modified from the original Livox ROS Driver2 source
 * refactored build, and multi-sensor support).
*/

#include <iostream>
#include <chrono>
#include <vector>
#include <csignal>
#include <thread>

#include "livox_node/livox_ros_driver2.h"
#include "livox_node/ros_headers.h"
#include "livox_node/driver_node.h"
#include "livox_node/lddc.h"
#include "livox_core/lds_lidar.h"

#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include <cstdio>

using namespace livox_ros;

namespace livox_ros
{
DriverNode::DriverNode(const rclcpp::NodeOptions & node_options)
                      : Node("livox_driver_node", node_options)
{
  DRIVER_INFO(*this, "Livox Ros Driver2 Version: %s", LIVOX_ROS_DRIVER2_VERSION_STRING);

  // --- Declare Parameters with Defaults ---
  std::string user_config_path = this->declare_parameter<std::string>("user_config_path", "path_default");
  double publish_freq = GetPublishFreqFromConfig(user_config_path, this->get_logger());
  
  DRIVER_INFO(*this, "Config file : %s", user_config_path.c_str());

  // --- Initialize Future for Thread Exit ---
  future_ = exit_signal_.get_future();

  // --- Initialize LDDS and Lidar Source ---
  lddc_ptr_ = std::make_unique<Lddc>();
  lddc_ptr_->SetRosNode(this);

  LdsLidar *read_lidar = LdsLidar::GetInstance(publish_freq);
  lddc_ptr_->RegisterLds(static_cast<Lds *>(read_lidar));

  if ((read_lidar->InitLdsLidar(user_config_path))) {
    DRIVER_INFO(*this, "Init lds lidar success!");
  } else {
    DRIVER_ERROR(*this, "Init lds lidar fail!");
  }

  post_init_timer_ = this->create_wall_timer(
    std::chrono::milliseconds(200),
    [this]() {
      try {
        this->OnAfterConstruction();
      } catch (const std::exception &e) {
        DRIVER_ERROR(*this, "OnAfterConstruction exception: %s", e.what());
      } 
    }
  );

  pointclouddata_poll_thread_ = std::make_shared<std::thread>(&DriverNode::PointCloudDataPollThread, this);
  imudata_poll_thread_ = std::make_shared<std::thread>(&DriverNode::ImuDataPollThread, this);
}

void DriverNode::PointCloudDataPollThread()
{
  std::this_thread::sleep_for(std::chrono::seconds(3));
  while (future_.wait_for(std::chrono::microseconds(0)) == std::future_status::timeout) {
    lddc_ptr_->DistributePointCloudData();
  }
}

void DriverNode::ImuDataPollThread()
{
  std::this_thread::sleep_for(std::chrono::seconds(3));
  while (future_.wait_for(std::chrono::microseconds(0)) == std::future_status::timeout) {
    lddc_ptr_->DistributeImuData();
  }
}

void DriverNode::OnAfterConstruction() {
  if (!lddc_ptr_) {
    DRIVER_WARN(*this, "LDDC not initialized — TFs not published.");
    return;
  }

  try {
    auto self = this->shared_from_this();  // Throws bad_weak_ptr if not managed
    lddc_ptr_->InitializeTFBroadcaster(self);
    lddc_ptr_->PublishTFs();
  } catch (const std::bad_weak_ptr &e) {
    DRIVER_ERROR(*this, "Cannot publish TFs — DriverNode not yet fully managed by shared_ptr: %s", e.what());
  } catch (const std::exception &e) {
    DRIVER_ERROR(*this, "Unexpected error while publishing TFs: %s", e.what());
  }
}

double DriverNode::GetPublishFreqFromConfig(const std::string &config_path, rclcpp::Logger logger) {
  FILE* fp = fopen(config_path.c_str(), "r");
  if (!fp) {
    RCLCPP_WARN(logger, "Could not open config file: %s. Using default 10.0 Hz.", config_path.c_str());
    return 10.0;
  }

  char readBuffer[65536];
  rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));
  rapidjson::Document doc;
  doc.ParseStream(is);
  fclose(fp);

  if (doc.HasParseError()) {
    RCLCPP_WARN(logger, "JSON parse error in %s. Using default 10.0 Hz.", config_path.c_str());
    return 10.0;
  }

  double publish_freq = 10.0; // default fallback

  // --- Case 1: Standard Livox config with multiple lidar_configs ---
  if (doc.HasMember("lidar_configs") && doc["lidar_configs"].IsArray()) {
    const auto& lidar_array = doc["lidar_configs"];
    double sum = 0.0;
    int count = 0;

    for (const auto& lidar_cfg : lidar_array.GetArray()) {
      if (lidar_cfg.HasMember("publish_frq") && lidar_cfg["publish_frq"].IsNumber()) {
        sum += lidar_cfg["publish_frq"].GetDouble();
        count++;
      }
    }

    if (count > 0) {
      publish_freq = sum / count;
      RCLCPP_INFO(logger, "Loaded average publish_frq = %.2f Hz from %d LiDAR configs", publish_freq, count);
      return publish_freq;
    } else {
      RCLCPP_WARN(logger, "No publish_frq found in any lidar_configs entry. Using default 10.0 Hz.");
      return publish_freq;
    }
  }

  // --- Case 2: Fallback for single-frequency config ---
  if (doc.HasMember("publish_frq") && doc["publish_frq"].IsNumber()) {
    publish_freq = doc["publish_frq"].GetDouble();
    RCLCPP_INFO(logger, "Loaded publish_frq = %.2f Hz from config root", publish_freq);
  } else {
    RCLCPP_WARN(logger, "publish_frq not found in %s. Using default 10.0 Hz.", config_path.c_str());
  }

  return publish_freq;
}

}  // namespace livox_ros

#include <rclcpp_components/register_node_macro.hpp>
RCLCPP_COMPONENTS_REGISTER_NODE(livox_ros::DriverNode)
