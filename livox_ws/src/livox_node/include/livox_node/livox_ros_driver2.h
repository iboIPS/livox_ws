/*
 * MIT License
 *
 * Copyright (c) 2022 Livox Tech. Company Limited
 * Copyright (c) 2025 Ibrahim Ismael
 *
 * This file has been modified from the original Livox ROS Driver2 source
 * refactored build, and multi-sensor support).
 */

#ifndef LIVOX_ROS_DRIVER2_INClUDE_H_
#define LIVOX_ROS_DRIVER2_INClUDE_H_

#define LIVOX_ROS_DRIVER2_VER_MAJOR 1
#define LIVOX_ROS_DRIVER2_VER_MINOR 2
#define LIVOX_ROS_DRIVER2_VER_PATCH 4

#define GET_STRING(n) GET_STRING_DIRECT(n)
#define GET_STRING_DIRECT(n) #n

#define LIVOX_ROS_DRIVER2_VERSION_STRING                      \
  GET_STRING(LIVOX_ROS_DRIVER2_VER_MAJOR)                     \
  "." GET_STRING(LIVOX_ROS_DRIVER2_VER_MINOR) "." GET_STRING( \
      LIVOX_ROS_DRIVER2_VER_PATCH)

#endif  // LIVOX_ROS_DRIVER2_INClUDE_H_
