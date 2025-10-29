/*
 * MIT License
 *
 * Copyright (c) 2022 Livox Tech. Company Limited
 * Copyright (c) 2025 Ibrahim Ismael
 *
 * This file has been modified from the original Livox ROS Driver2 source
 * refactored build, and multi-sensor support).
*/

#ifndef ROS_HEADERS_H_
#define ROS_HEADERS_H_

#include <thread>
#include <future>

#include <rclcpp/rclcpp.hpp>
#include <pcl_conversions/pcl_conversions.h>
#include <sensor_msgs/msg/point_cloud2.hpp>
#include <sensor_msgs/msg/imu.hpp>
#include <tf2_ros/static_transform_broadcaster.h>
#include <geometry_msgs/msg/transform_stamped.hpp>
#include <tf2/LinearMath/Quaternion.h>


#define DRIVER_DEBUG(node, ...) RCLCPP_DEBUG((node).get_logger(), __VA_ARGS__)
#define DRIVER_INFO(node, ...) RCLCPP_INFO((node).get_logger(), __VA_ARGS__)
#define DRIVER_WARN(node, ...) RCLCPP_WARN((node).get_logger(), __VA_ARGS__)
#define DRIVER_ERROR(node, ...) RCLCPP_ERROR((node).get_logger(), __VA_ARGS__)
#define DRIVER_FATAL(node, ...) RCLCPP_FATAL((node).get_logger(), __VA_ARGS__)

#define DRIVER_DEBUG_EXTRA(node, EXTRA, ...) RCLCPP_DEBUG_##EXTRA((node).get_logger(), __VA_ARGS__)
#define DRIVER_INFO_EXTRA(node, EXTRA, ...) RCLCPP_INFO_##EXTRA((node).get_logger(), __VA_ARGS__)
#define DRIVER_WARN_EXTRA(node, EXTRA, ...) RCLCPP_WARN_##EXTRA((node).get_logger(), __VA_ARGS__)
#define DRIVER_ERROR_EXTRA(node, EXTRA, ...) RCLCPP_ERROR_##EXTRA((node).get_logger(), __VA_ARGS__)
#define DRIVER_FATAL_EXTRA(node, EXTRA, ...) RCLCPP_FATAL_##EXTRA((node).get_logger(), __VA_ARGS__)

#endif // ROS_HEADERS_H_
