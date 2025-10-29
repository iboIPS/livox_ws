/*
 * MIT License
 *
 * Copyright (c) 2022 Livox Tech. Company Limited
 * Copyright (c) 2025 Ibrahim Ismael
 *
 * This file has been modified from the original Livox ROS Driver2 source
 * refactored build, and multi-sensor support).
 */

#ifndef LIVOX_ROS_DRIVER_LDQ_H_
#define LIVOX_ROS_DRIVER_LDQ_H_

#include <stdint.h>
#include <vector>

#include "livox_core/comm/comm.h"

namespace livox_ros {
  
inline static bool IsPowerOf2(uint32_t size) {
  return (size != 0) && ((size & (size - 1)) == 0);
}

inline static uint32_t RoundupPowerOf2(uint32_t size) {
  uint32_t power2_val = 0;
  for (int i = 0; i < 32; i++) {
    power2_val = ((uint32_t)1) << i;
    if (size <= power2_val) {
      break;
    }
  }

  return power2_val;
}

/** queue operate function */
bool InitQueue(LidarDataQueue *queue, uint32_t queue_size);
bool DeInitQueue(LidarDataQueue *queue);
void ResetQueue(LidarDataQueue *queue);
bool QueuePrePop(LidarDataQueue *queue, StoragePacket *storage_packet);
void QueuePopUpdate(LidarDataQueue *queue);
bool QueuePop(LidarDataQueue *queue, StoragePacket *storage_packet);
uint32_t QueueUsedSize(LidarDataQueue *queue);
uint32_t QueueUnusedSize(LidarDataQueue *queue);
bool QueueIsFull(LidarDataQueue *queue);
bool QueueIsEmpty(LidarDataQueue *queue);
uint32_t QueuePushAny(LidarDataQueue *queue, uint8_t *data, const uint64_t base_time);

}  // namespace livox_ros

#endif // LIVOX_ROS_DRIVER_LDQ_H_
