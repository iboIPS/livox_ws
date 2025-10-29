/*
 * MIT License
 *
 * Copyright (c) 2022 Livox Tech. Company Limited
 * Copyright (c) 2025 Ibrahim Ismael
 *
 * This file has been modified from the original Livox ROS Driver2 source
 * refactored build, and multi-sensor support).
*/

#include "parse_cfg_file/parse_livox_lidar_cfg.h"
#include <iostream>

namespace livox_ros {

bool LivoxLidarConfigParser::Parse(std::vector<UserLivoxLidarConfig> &lidar_configs) {
  FILE* raw_file = std::fopen(path_.c_str(), "rb");
  if (!raw_file) {
    std::cout << "failed to open config file: " << path_ << std::endl;
    return false;
  }

  lidar_configs.clear();
  char read_buffer[kMaxBufferSize];
  rapidjson::FileReadStream config_file(raw_file, read_buffer, sizeof(read_buffer));
  rapidjson::Document doc;

  do {
    if (doc.ParseStream(config_file).HasParseError()) {
      std::cout << "failed to parse config jason" << std::endl;
      break;
    }
    if (!doc.HasMember("lidar_configs") ||
        !doc["lidar_configs"].IsArray() ||
        0 == doc["lidar_configs"].Size()) {
      std::cout << "there is no user-defined config" << std::endl;
      break;
    }
    if (!ParseUserConfigs(doc, lidar_configs)) {
      std::cout << "failed to parse basic configs" << std::endl;
      break;
    }
    return true;
  } while (false);

  std::fclose(raw_file);
  return false;
}

bool LivoxLidarConfigParser::ParseUserConfigs(const rapidjson::Document &doc,
                                              std::vector<UserLivoxLidarConfig> &user_configs) {
  const rapidjson::Value &lidar_configs = doc["lidar_configs"];
  for (auto &config : lidar_configs.GetArray()) {
    if (!config.HasMember("ip")) {
      continue;
    }
    UserLivoxLidarConfig user_config;

    // parse user configs
    user_config.handle = IpStringToNum(std::string(config["ip"].GetString()));
    if (!config.HasMember("pcl_data_type")) {
      user_config.pcl_data_type = -1;
    } else {
      user_config.pcl_data_type = static_cast<int8_t>(config["pcl_data_type"].GetInt());
    }
    if (!config.HasMember("frame_id")) {
      user_config.frame_id = "livox_frame";
    } else {
      user_config.frame_id = std::string(config["frame_id"].GetString());
    }
    if (!config.HasMember("parent_frame_id")) {
      user_config.parent_frame_id = "base_link";
    } else {
      user_config.parent_frame_id = std::string(config["parent_frame_id"].GetString());
    }
    if (!config.HasMember("topic_name")) {
      user_config.topic_name = "livox";
    } else {
      user_config.topic_name = std::string(config["topic_name"].GetString());
    }
    if (!config.HasMember("publish_frq")){
      user_config.publish_frq = 50;
    } else {
      user_config.publish_frq = double(config["publish_frq"].GetDouble());
    }
    if (!config.HasMember("publish_tf")){
      user_config.publish_tf = true;
    } else {
      user_config.publish_tf = double(config["publish_tf"].GetBool());
    }
    if (!config.HasMember("pattern_mode")) {
      user_config.pattern_mode = -1;
    } else {
      user_config.pattern_mode = static_cast<int8_t>(config["pattern_mode"].GetInt());
    }
    if (!config.HasMember("blind_spot_set")) {
      user_config.blind_spot_set = -1;
    } else {
      user_config.blind_spot_set = static_cast<int8_t>(config["blind_spot_set"].GetInt());
    }
    if (!config.HasMember("dual_emit_en")) {
      user_config.dual_emit_en = -1;
    } else {
      user_config.dual_emit_en = static_cast<uint8_t>(config["dual_emit_en"].GetInt());
    }
    if (!config.HasMember("extrinsic_parameter")) {
      memset(&user_config.extrinsic_param, 0, sizeof(user_config.extrinsic_param));
    } else {
      auto &value = config["extrinsic_parameter"];
      if (!ParseExtrinsics(value, user_config.extrinsic_param)) {
        memset(&user_config.extrinsic_param, 0, sizeof(user_config.extrinsic_param));
        std::cout << "failed to parse extrinsic parameters, ip: "
                  << IpNumToString(user_config.handle) << std::endl;
      }
    }
    user_config.set_bits = 0;
    user_config.get_bits = 0;

    user_configs.push_back(user_config);
  }

  if (0 == user_configs.size()) {
    std::cout << "no valid base configs" << std::endl;
    return false;
  }
  std::cout << "successfully parse base config, counts: "
            << user_configs.size() << std::endl;
  return true;
}

bool LivoxLidarConfigParser::ParseExtrinsics(const rapidjson::Value &value,
                                             ExtParameter &param) {
  auto getFloatSafe = [](const rapidjson::Value &v, const char *key, float default_val) -> float {
    if (v.HasMember(key)) {
      const auto &x = v[key];
      if (x.IsDouble()) return static_cast<float>(x.GetDouble());
      if (x.IsFloat())  return x.GetFloat();
      if (x.IsInt())    return static_cast<float>(x.GetInt());
      if (x.IsString()) {
        try { return std::stof(x.GetString()); }
        catch (...) { return default_val; }
      }
    }
    return default_val;
  };

  param.roll  = getFloatSafe(value, "roll", 0.0f);
  param.pitch = getFloatSafe(value, "pitch", 0.0f);
  param.yaw   = getFloatSafe(value, "yaw", 0.0f);
  param.x     = getFloatSafe(value, "x", 0.0f);
  param.y     = getFloatSafe(value, "y", 0.0f);
  param.z     = getFloatSafe(value, "z", 0.0f);

  return true;
}

} // namespace livox_ros
