/**
 * @file main.cpp
 * @author LDRobot (support@ldrobot.com)
 * @brief  main process App
 *         This code is only applicable to LDROBOT LiDAR LD00 LD03 LD08 LD14
 * products sold by Shenzhen LDROBOT Co., LTD
 * @version 0.1
 * @date 2021-11-08
 *
 * @copyright Copyright (c) 2021  SHENZHEN LDROBOT CO., LTD. All rights
 * reserved.
 * Licensed under the MIT License (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License in the file LICENSE
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "ldlidar_driver.h"

uint64_t GetTimestamp(void) {
  std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds> tp = 
    std::chrono::time_point_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now());
  auto tmp = std::chrono::duration_cast<std::chrono::nanoseconds>(tp.time_since_epoch());
  return ((uint64_t)tmp.count());
}

// void LidarPowerOn(void) {
//   LDS_LOG_DEBUG("Lidar Power On","");
//   // ...
// }

// void LidarPowerOff(void) {
//   LDS_LOG_DEBUG("Lidar Power Off","");
//   // ...
// }

class LdsParamInit {
 public:
  struct LdsInfoStruct {
    std::string ldtype_str;
    ldlidar::LDType ldtype_enum;
    uint32_t baudrate;
  } LdsInfoArrary[3];

  LdsParamInit() {
    LdsInfoArrary[0] = {"LD14", ldlidar::LDType::LD_14, 115200};
    LdsInfoArrary[1] = {"LD14P_2300HZ", ldlidar::LDType::LD_14P_2300HZ, 230400};
    LdsInfoArrary[2] = {"LD14P_4000HZ", ldlidar::LDType::LD_14P_4000HZ, 230400};
  }

  ldlidar::LDType GetLdsType(std::string in_str) {
    for (auto item : LdsInfoArrary) {
      if (!strcmp(in_str.c_str(), item.ldtype_str.c_str())) {
        return item.ldtype_enum;
      }
    }
    return ldlidar::LDType::NO_VER;
  }

  uint32_t GetLdsPortBaudrateVal(std::string in_str) {
    for (auto item : LdsInfoArrary) {
      if (!strcmp(in_str.c_str(), item.ldtype_str.c_str())) {
        return item.baudrate;
      }
    }
    return 0;
  }
};

int main(int argc, char **argv) {
  
  if (argc != 3) {
    LDS_LOG_INFO("cmd error","");
    LDS_LOG_INFO("please input: ./ldlidar_sl_node <product_type> <serial_number>","");
    LDS_LOG_INFO("example:","");
    LDS_LOG_INFO("./ldlidar_sl_node LD14 /dev/ttyUSB0","");
    LDS_LOG_INFO("./ldlidar_sl_node LD14P_2300HZ /dev/ttyUSB0","");
    LDS_LOG_INFO("./ldlidar_sl_node LD14P_4000HZ /dev/ttyUSB0","");
    exit(EXIT_FAILURE);
  }
  
  std::string ldlidar_type_str(argv[1]);
  std::string port_name(argv[2]);
  LdsParamInit param_initialize;

  // select ldrobot lidar sensor type.
  ldlidar::LDType ldlidar_type_dest;
  ldlidar_type_dest = param_initialize.GetLdsType(ldlidar_type_str);
  if (ldlidar_type_dest == ldlidar::LDType::NO_VER) {
    LDS_LOG_ERROR("ldlidar_type_str value is not sure: %s", ldlidar_type_str.c_str());
    exit(EXIT_FAILURE);
  }
  // if use serial communications interface, as select serial baudrate paramters.
  uint32_t baudrate_val;
  baudrate_val = param_initialize.GetLdsPortBaudrateVal(ldlidar_type_str);
  if (!baudrate_val) {
    LDS_LOG_ERROR("ldlidar_type_str value is not sure: %s", ldlidar_type_str.c_str());
    exit(EXIT_FAILURE);
  }
  
  ldlidar::LDLidarDriver* node = new ldlidar::LDLidarDriver();
  
  LDS_LOG_INFO("LDLiDAR SDK Pack Version is %s", node->GetLidarSdkVersionNumber().c_str());

  node->RegisterGetTimestampFunctional(std::bind(&GetTimestamp)); 

  node->EnableFilterAlgorithnmProcess(true);

  if (node->Start(ldlidar_type_dest, port_name, baudrate_val)) {
    LDS_LOG_INFO("ldlidar node start is success","");
    // LidarPowerOn();
  } else {
    LDS_LOG_ERROR("ldlidar node start is fail","");
    exit(EXIT_FAILURE);
  }

  if (node->WaitLidarCommConnect(3500)) {
    LDS_LOG_INFO("ldlidar communication is normal.","");
  } else {
    LDS_LOG_ERROR("ldlidar communication is abnormal.","");
    node->Stop();
  }
  
  ldlidar::Points2D laser_scan_points;
  while (ldlidar::LDLidarDriver::IsOk()) {

    switch (node->GetLaserScanData(laser_scan_points, 1500)){
      case ldlidar::LidarStatus::NORMAL: {
        double lidar_scan_freq = 0;
        node->GetLidarScanFreq(lidar_scan_freq);
#ifdef __LP64__
        LDS_LOG_INFO("speed(Hz):%f, size:%d,stamp_front:%lu, stamp_back:%lu",
            lidar_scan_freq, laser_scan_points.size(), laser_scan_points.front().stamp, laser_scan_points.back().stamp);
#else
        LDS_LOG_INFO("speed(Hz):%f, size:%d,stamp_front:%llu, stamp_back:%llu",
            lidar_scan_freq, laser_scan_points.size(), laser_scan_points.front().stamp, laser_scan_points.back().stamp);
#endif

#if 0
        //  output 2d point cloud data
        for (auto point : laser_scan_points) {
#ifdef __LP64__
          LDS_LOG_INFO("stamp(ns):%lu,angle:%f,distance(mm):%d,intensity:%d", 
              point.stamp, point.angle, point.distance, point.intensity);
#else
          LDS_LOG_INFO("stamp(ns):%llu,angle:%f,distance(mm):%d,intensity:%d", 
              point.stamp, point.angle, point.distance, point.intensity);
#endif
        }
#endif
        break;
      }
      case ldlidar::LidarStatus::DATA_TIME_OUT: {
        LDS_LOG_ERROR("ldlidar point cloud data publish time out, please check your lidar device.","");
        node->Stop();
        break;
      }
      case ldlidar::LidarStatus::DATA_WAIT: {
        break;
      }
      default:
        break;
    }

    usleep(1000*166);  // sleep 166ms , 6hz
  }

  node->Stop();
  // LidarPowerOff();

  delete node;
  node = nullptr;

  return 0;
}

/********************* (C) COPYRIGHT SHENZHEN LDROBOT CO., LTD *******END OF
 * FILE ********/
