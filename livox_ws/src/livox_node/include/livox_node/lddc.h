/*
 * MIT License
 *
 * Copyright (c) 2022 Livox Tech. Company Limited
 * Copyright (c) 2025 Ibrahim Ismael
 *
 * This file has been modified from the original Livox ROS Driver2 source
 * refactored build, and multi-sensor support).
*/

#ifndef LIVOX_ROS_DRIVER2_LDDC_H_
#define LIVOX_ROS_DRIVER2_LDDC_H_

#include "livox_ros_driver2.h"
#include "driver_node.h"
#include "livox_core/lds.h"

#include <vector>

namespace livox_ros {

/** The message type of transfer */
typedef enum {
  kPointCloud2Msg = 0,
  kLivoxImuMsg = 3,
} TransferType;

template <typename MessageT> using Publisher = rclcpp::Publisher<MessageT>;
using PublisherPtr = std::shared_ptr<rclcpp::PublisherBase>;
using PointCloud2 = sensor_msgs::msg::PointCloud2;
using PointField = sensor_msgs::msg::PointField;
using ImuMsg = sensor_msgs::msg::Imu;


using PointCloud = pcl::PointCloud<pcl::PointXYZI>;

class DriverNode;

class Lddc final {
 public:
  Lddc();
  ~Lddc();

  int RegisterLds(Lds *lds);
  void DistributePointCloudData(void);
  void DistributeImuData(void);
  void PrepareExit(void);

  uint8_t GetTransferFormat(void) { return transfer_format_; }
  uint8_t IsMultiTopic(void) { return use_multi_topic_; }
  void SetRosNode(livox_ros::DriverNode *node) { cur_node_ = node; }

  void SetPublishFrq(uint32_t frq) { publish_frq_ = frq; }
  void InitializeTFBroadcaster(std::shared_ptr<rclcpp::Node> node);
  void PublishTFs();

 public:
  Lds *lds_;

 private:
  void PollingLidarPointCloudData(uint8_t index, LidarDevice *lidar);
  void PollingLidarImuData(uint8_t index, LidarDevice *lidar);

  void PublishPointcloud2(LidarDataQueue *queue, uint8_t index);

  void PublishImuData(LidarImuDataQueue& imu_data_queue, const uint8_t index);

  void InitPointcloud2MsgHeader(PointCloud2& cloud);
  void InitPointcloud2Msg(const uint8_t index, const StoragePacket& pkg, PointCloud2& cloud, uint64_t& timestamp);
  void PublishPointcloud2Data(const uint8_t index, uint64_t timestamp, const PointCloud2& cloud);

  void InitImuMsg(const uint8_t index, const ImuData& imu_data, ImuMsg& imu_msg, uint64_t& timestamp);
  
  PublisherPtr CreatePublisher(uint8_t msg_type, std::string &topic_name, uint32_t queue_size);

  PublisherPtr GetCurrentPublisher(uint8_t index);
  PublisherPtr GetCurrentImuPublisher(uint8_t index);

 private:
  uint8_t transfer_format_;
  uint8_t use_multi_topic_;
  double publish_frq_;
  std::string frame_id_;
  std::vector<std::string> frame_ids_;  // size = kMaxSourceLidar
  bool publish_tf_;

  PublisherPtr private_pub_[kMaxSourceLidar];
  PublisherPtr global_pub_;
  PublisherPtr private_imu_pub_[kMaxSourceLidar];
  PublisherPtr global_imu_pub_;

  livox_ros::DriverNode *cur_node_;
  std::shared_ptr<tf2_ros::StaticTransformBroadcaster> static_tf_broadcaster_;
};

}  // namespace livox_ros

#endif // LIVOX_ROS_DRIVER2_LDDC_H_
