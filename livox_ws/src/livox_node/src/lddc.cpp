/*
 * MIT License
 *
 * Copyright (c) 2022 Livox Tech. Company Limited
 * Copyright (c) 2025 Ibrahim Ismael
 *
 * This file has been modified from the original Livox ROS Driver2 source
 * refactored build, and multi-sensor support).
*/

#include "livox_node/lddc.h"
#include "livox_core/comm/ldq.h"
#include "livox_core/comm/comm.h"

#include <inttypes.h>
#include <iostream>
#include <iomanip>
#include <math.h>
#include <stdint.h>

#include "livox_node/ros_headers.h"
#include "livox_node/driver_node.h"
#include "livox_core/lds_lidar.h"

namespace livox_ros {

/** Lidar Data Distribute Control--------------------------------------------*/
Lddc::Lddc() {
  lds_ = nullptr;
}

Lddc::~Lddc() {
  PrepareExit();
  DRIVER_INFO(*cur_node_, "lddc destory!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
}

int Lddc::RegisterLds(Lds *lds) {
  if (lds_ == nullptr) {
    lds_ = lds;
    frame_ids_.resize(kMaxSourceLidar, lds_->lidars_->livox_config.frame_id); // deafult all same
    return 0;
  } else {
    return -1;
  }
}

void Lddc::DistributePointCloudData(void) {
  if (!lds_) {
    DRIVER_INFO(*cur_node_, "lds is not registered");
    return;
  }
  if (lds_->IsRequestExit()) {
    DRIVER_INFO(*cur_node_, "DistributePointCloudData is RequestExit");
    return;
  }
  
  lds_->pcd_semaphore_.Wait();
  for (uint32_t i = 0; i < lds_->lidar_count_; i++) {
    uint32_t lidar_id = i;
    LidarDevice *lidar = &lds_->lidars_[lidar_id];
    LidarDataQueue *p_queue = &lidar->data;
    if ((kConnectStateSampling != lidar->connect_state) || (p_queue == nullptr)) {
      continue;
    }
    PollingLidarPointCloudData(lidar_id, lidar);    
  }
}

void Lddc::DistributeImuData(void) {
  if (!lds_) {
    DRIVER_INFO(*cur_node_, "lds is not registered");
    return;
  }
  if (lds_->IsRequestExit()) {
    DRIVER_INFO(*cur_node_, "DistributeImuData is RequestExit");
    return;
  }
  
  lds_->imu_semaphore_.Wait();
  for (uint32_t i = 0; i < lds_->lidar_count_; i++) {
    uint32_t lidar_id = i;
    LidarDevice *lidar = &lds_->lidars_[lidar_id];
    LidarImuDataQueue *p_queue = &lidar->imu_data;
    if ((kConnectStateSampling != lidar->connect_state) || (p_queue == nullptr)) {
      continue;
    }
    PollingLidarImuData(lidar_id, lidar);
  }
}

void Lddc::PollingLidarPointCloudData(uint8_t index, LidarDevice *lidar) {
  LidarDataQueue *p_queue = &lidar->data;
  if (p_queue == nullptr || p_queue->storage_packet == nullptr) {
    return;
  }

  while (!lds_->IsRequestExit() && !QueueIsEmpty(p_queue)) {
    PublishPointcloud2(p_queue, index);
  }
}

void Lddc::PollingLidarImuData(uint8_t index, LidarDevice *lidar) {
  LidarImuDataQueue& p_queue = lidar->imu_data;
  while (!lds_->IsRequestExit() && !p_queue.Empty()) {
    PublishImuData(p_queue, index);
  }
}

void Lddc::PrepareExit(void) {
  if (lds_) {
    lds_->PrepareExit();
    lds_ = nullptr;
  }
}

void Lddc::PublishPointcloud2(LidarDataQueue *queue, uint8_t index) {
  while(!QueueIsEmpty(queue)) {
    StoragePacket pkg;
    QueuePop(queue, &pkg);
    if (pkg.points.empty()) {
      DRIVER_FATAL(*cur_node_, "Publish point cloud2 failed, the pkg points is empty.");
      continue;
    }

    PointCloud2 cloud;
    uint64_t timestamp = 0;
    InitPointcloud2Msg(index, pkg, cloud, timestamp);
    PublishPointcloud2Data(index, timestamp, cloud);
  }
}

void Lddc::InitPointcloud2MsgHeader(PointCloud2& cloud) {
  cloud.header.frame_id.assign(frame_id_);
  cloud.height = 1;
  cloud.width = 0;
  cloud.fields.resize(7);
  cloud.fields[0].offset = 0;
  cloud.fields[0].name = "x";
  cloud.fields[0].count = 1;
  cloud.fields[0].datatype = PointField::FLOAT32;
  cloud.fields[1].offset = 4;
  cloud.fields[1].name = "y";
  cloud.fields[1].count = 1;
  cloud.fields[1].datatype = PointField::FLOAT32;
  cloud.fields[2].offset = 8;
  cloud.fields[2].name = "z";
  cloud.fields[2].count = 1;
  cloud.fields[2].datatype = PointField::FLOAT32;
  cloud.fields[3].offset = 12;
  cloud.fields[3].name = "intensity";
  cloud.fields[3].count = 1;
  cloud.fields[3].datatype = PointField::FLOAT32;
  cloud.fields[4].offset = 16;
  cloud.fields[4].name = "tag";
  cloud.fields[4].count = 1;
  cloud.fields[4].datatype = PointField::UINT8;
  cloud.fields[5].offset = 17;
  cloud.fields[5].name = "line";
  cloud.fields[5].count = 1;
  cloud.fields[5].datatype = PointField::UINT8;
  cloud.fields[6].offset = 18;
  cloud.fields[6].name = "timestamp";
  cloud.fields[6].count = 1;
  cloud.fields[6].datatype = PointField::FLOAT64;
  cloud.point_step = sizeof(LivoxPointXyzrtlt);
}

void Lddc::InitPointcloud2Msg(const uint8_t index, const StoragePacket& pkg, PointCloud2& cloud, uint64_t& timestamp) {
  InitPointcloud2MsgHeader(cloud);

  // // --- Frame ID selection ---
  std::string lidar_frame;
  if (lds_->lidars_[index].livox_config.publish_tf) {
    lidar_frame = lds_->lidars_[index].livox_config.parent_frame_id;
  } else {
    lidar_frame = lds_->lidars_[index].livox_config.frame_id;
  }

  cloud.header.frame_id.assign(lidar_frame);
  cloud.point_step = sizeof(LivoxPointXyzrtlt);
  cloud.width = pkg.points_num;
  cloud.row_step = cloud.width * cloud.point_step;
  cloud.is_bigendian = false;
  cloud.is_dense     = true;

  if (!pkg.points.empty()) {
    timestamp = pkg.base_time;
  }

  cloud.header.stamp = rclcpp::Time(timestamp);

  std::vector<LivoxPointXyzrtlt> points;
  for (size_t i = 0; i < pkg.points_num; ++i) {
    LivoxPointXyzrtlt point;
    point.x = pkg.points[i].x;
    point.y = pkg.points[i].y;
    point.z = pkg.points[i].z;
    point.reflectivity = pkg.points[i].intensity;
    point.tag = pkg.points[i].tag;
    point.line = pkg.points[i].line;
    point.timestamp = static_cast<double>(pkg.points[i].offset_time);
    points.push_back(std::move(point));
  }
  cloud.data.resize(pkg.points_num * sizeof(LivoxPointXyzrtlt));
  memcpy(cloud.data.data(), points.data(), pkg.points_num * sizeof(LivoxPointXyzrtlt));
}

void Lddc::PublishPointcloud2Data(const uint8_t index, const uint64_t timestamp, const PointCloud2& cloud) {
  (void)timestamp;
  Publisher<PointCloud2>::SharedPtr publisher_ptr =
    std::dynamic_pointer_cast<Publisher<PointCloud2>>(GetCurrentPublisher(index));
  publisher_ptr->publish(cloud);
}

void Lddc::InitImuMsg(const uint8_t index, const ImuData& imu_data, ImuMsg& imu_msg, uint64_t& timestamp) {
  imu_msg.header.frame_id = lds_->lidars_[index].livox_config.frame_id;

  timestamp = imu_data.time_stamp;
  imu_msg.header.stamp = rclcpp::Time(timestamp);  // to ros time stamp

  imu_msg.angular_velocity.x = imu_data.gyro_x;
  imu_msg.angular_velocity.y = imu_data.gyro_y;
  imu_msg.angular_velocity.z = imu_data.gyro_z;
  imu_msg.linear_acceleration.x = imu_data.acc_x;
  imu_msg.linear_acceleration.y = imu_data.acc_y;
  imu_msg.linear_acceleration.z = imu_data.acc_z;
}

void Lddc::PublishImuData(LidarImuDataQueue& imu_data_queue, const uint8_t index) {
  ImuData imu_data;
  if (!imu_data_queue.Pop(imu_data)) {
    return;
  }

  ImuMsg imu_msg;
  uint64_t timestamp;
  InitImuMsg(index, imu_data, imu_msg, timestamp);

  Publisher<ImuMsg>::SharedPtr publisher_ptr = std::dynamic_pointer_cast<Publisher<ImuMsg>>(GetCurrentImuPublisher(index));
  publisher_ptr->publish(imu_msg);
}

std::shared_ptr<rclcpp::PublisherBase> Lddc::CreatePublisher(uint8_t msg_type, std::string &topic_name, uint32_t queue_size) {
  rclcpp::QoS qos = rclcpp::SensorDataQoS().keep_last(queue_size);

  if (kPointCloud2Msg == msg_type) {
    DRIVER_INFO(*cur_node_,
        "%s publish use PointCloud2 format", topic_name.c_str());
    return cur_node_->create_publisher<PointCloud2>(topic_name, qos);
  } 
  else if (kLivoxImuMsg == msg_type)  {
    DRIVER_INFO(*cur_node_,
        "%s publish use imu format", topic_name.c_str());
    return cur_node_->create_publisher<ImuMsg>(topic_name, qos);
  } else {
    PublisherPtr null_publisher(nullptr);
    return null_publisher;
  }
}

std::shared_ptr<rclcpp::PublisherBase> Lddc::GetCurrentPublisher(uint8_t handle) {
  uint32_t queue_size = lds_->lidars_[handle].livox_config.qos_queue_size;
  if (!private_pub_[handle]) {
    std::string topic_name(lds_->lidars_[handle].livox_config.topic_name + "/lidar");
    private_pub_[handle] = CreatePublisher(kPointCloud2Msg, topic_name, queue_size);
  }
  return private_pub_[handle];
}

std::shared_ptr<rclcpp::PublisherBase> Lddc::GetCurrentImuPublisher(uint8_t handle) {
  uint32_t queue_size = lds_->lidars_[handle].livox_config.qos_queue_size;
  if (!private_imu_pub_[handle]) {
    std::string topic_name(lds_->lidars_[handle].livox_config.topic_name + "/imu");
    private_imu_pub_[handle] = CreatePublisher(kLivoxImuMsg, topic_name, queue_size);
  }
  return private_imu_pub_[handle];
}

void Lddc::InitializeTFBroadcaster(std::shared_ptr<rclcpp::Node> node) {
  if (!static_tf_broadcaster_) {
    static_tf_broadcaster_ =
      std::make_shared<tf2_ros::StaticTransformBroadcaster>(node);
    RCLCPP_INFO(node->get_logger(), "Static TF Broadcaster initialized.");
  }
}

void Lddc::PublishTFs() {
  if (!static_tf_broadcaster_ || !lds_) {
    return;
  }

  std::vector<geometry_msgs::msg::TransformStamped> transforms;
  transforms.reserve(lds_->lidar_count_);

  for (size_t i = 0; i < lds_->lidar_count_; ++i) {
    const auto &lidar = lds_->lidars_[i];

    // Skip disconnected or unconfigured lidar
    if (lidar.connect_state != kConnectStateSampling)
      continue;

    // Skip if publish_tf set to false
    if (!lidar.livox_config.publish_tf)
      continue;

    const auto &ext = lidar.livox_config.extrinsic_param;

    geometry_msgs::msg::TransformStamped tf_msg;
    tf_msg.header.stamp = cur_node_->get_clock()->now();

    // Parent and child frame names
    std::string parent = lidar.livox_config.parent_frame_id.empty() ? "base_link" : lidar.livox_config.parent_frame_id;
    std::string child = lidar.livox_config.frame_id.empty() ? frame_ids_[i] : lidar.livox_config.frame_id;

    tf_msg.header.frame_id = parent;
    tf_msg.child_frame_id = child;

    // --- 1. Convert translation from mm → m ---
    double tx = static_cast<double>(ext.x) / 1000.0;
    double ty = static_cast<double>(ext.y) / 1000.0;
    double tz = static_cast<double>(ext.z) / 1000.0;
    tf_msg.transform.translation.x = tx;
    tf_msg.transform.translation.y = ty;
    tf_msg.transform.translation.z = tz;

    // --- 2. Convert rotation from deg → rad ---
    double roll_rad  = static_cast<double>(ext.roll)  * M_PI / 180.0;
    double pitch_rad = static_cast<double>(ext.pitch) * M_PI / 180.0;
    double yaw_rad   = static_cast<double>(ext.yaw)   * M_PI / 180.0;

    // --- 3. Compute quaternion ---
    tf2::Quaternion q;
    q.setRPY(roll_rad, pitch_rad, yaw_rad);

    tf_msg.transform.rotation.x = q.x();
    tf_msg.transform.rotation.y = q.y();
    tf_msg.transform.rotation.z = q.z();
    tf_msg.transform.rotation.w = q.w();

    transforms.push_back(tf_msg);
  }

  // Send all transforms at once
  static_tf_broadcaster_->sendTransform(transforms);
}

}  // namespace livox_ros