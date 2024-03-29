/**
 * @file test.cpp
 * @author LDRobot (support@ldrobot.com)
 * @brief  sdk function test
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
#include <wiringPi.h>

uint64_t GetTimestamp(void) {
  std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds> tp = 
    std::chrono::time_point_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now());
  auto tmp = std::chrono::duration_cast<std::chrono::nanoseconds>(tp.time_since_epoch());
  return ((uint64_t)tmp.count());
}

bool  ControlPinInit(void) {
  // raspberrypi wiringPi GPIO Init
  if(wiringPiSetup() == -1) {
    return false;
  }
  pinMode(25,OUTPUT); // set wiringPi 25 Pin number is outuput Mode.
  return true;
}

void LidarPowerOn(void) {
  LDS_LOG_DEBUG("Lidar Power On","");
  digitalWrite(25,HIGH);
}

void LidarPowerOff(void) {
  LDS_LOG_DEBUG("Lidar Power Off","");
  digitalWrite(25,LOW);
}

void AbortTesting(void) {
  LDS_LOG_WARN("Testing abort","");
  LidarPowerOff();
  exit(EXIT_FAILURE);
}

int function_main(int argc, char **argv) {
  
  if (argc != 2) {
    LDS_LOG_INFO("cmd error","");
    LDS_LOG_INFO("please input: ./ldlidar_sl_node <serial_number>","");
    LDS_LOG_INFO("example:","");
    LDS_LOG_INFO("./ldlidar_sl_node /dev/ttyUSB0","");
    LDS_LOG_INFO("or","");
    LDS_LOG_INFO("./ldlidar_sl_node /dev/ttyS0","");
    exit(EXIT_FAILURE);
  }
  
  std::string port_name(argv[1]);
  
  ldlidar::LDLidarDriver* node = new ldlidar::LDLidarDriver();
  
  LDS_LOG_INFO("LDLiDAR SDK Pack Version is %s", node->GetLidarSdkVersionNumber().c_str());

  node->RegisterGetTimestampFunctional(std::bind(&GetTimestamp)); 

  node->EnableFilterAlgorithnmProcess(true);

  if (node->Start(ldlidar::LDType::LD_14, port_name)) {
    LDS_LOG_INFO("ldlidar node start is success","");
    LidarPowerOn();
  } else {
    LDS_LOG_ERROR("ldlidar node start is fail","");
    exit(EXIT_FAILURE);
  }

  if (node->WaitLidarCommConnect(3500)) {
    LDS_LOG_INFO("ldlidar communication is normal.","");
  } else {
    LDS_LOG_ERROR("ldlidar communication is abnormal.","");
    AbortTesting();
  }
  
  ldlidar::Points2D laser_scan_points;
  int cnt = 100;
  while (ldlidar::LDLidarDriver::IsOk()) {
    if ((cnt--) <= 0) {
      node->Stop();
    }

    switch (node->GetLaserScanData(laser_scan_points, 1500)){
      case ldlidar::LidarStatus::NORMAL: {
        double lidar_scan_freq = 0;
        node->GetLidarScanFreq(lidar_scan_freq);

#ifdef __LP64__
        LDS_LOG_INFO("speed(Hz):%f,size:%d,stamp_front:%lu, stamp_back:%lu",
            lidar_scan_freq, laser_scan_points.size(), laser_scan_points.front().stamp, laser_scan_points.back().stamp);
#else
        LDS_LOG_INFO("speed(Hz):%f,size:%d,stamp_front:%llu, stamp_back:%llu",
            lidar_scan_freq, laser_scan_points.size(), laser_scan_points.front().stamp, laser_scan_points.back().stamp);
#endif

        if (laser_scan_points.front().stamp >= laser_scan_points.back().stamp) {
          LDS_LOG_ERROR("timestamp error!","");
          node->Stop();
          AbortTesting();
        }
        
        int distance_zero_point_cnt = 0;
        for (auto point : laser_scan_points) {
          if (0 == point.distance) {
            distance_zero_point_cnt++;
          }
        }
        
        if (distance_zero_point_cnt >= (int)laser_scan_points.size()) {
          LDS_LOG_ERROR("a frame distance is zero value","");
          node->Stop();
          AbortTesting();
        }

        break;
      }
      case ldlidar::LidarStatus::DATA_TIME_OUT: {
        LDS_LOG_ERROR("ldlidar point cloud data publish time out, please check your lidar device.","");
        node->Stop();
        AbortTesting();
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
  LidarPowerOff();
  sleep(3);

  delete node;
  node = nullptr;

  return 0;
}

int main(int argc, char** argv) {

  if (!ControlPinInit()) {
    LDS_LOG_ERROR("Control pin Setup failed.","");
    exit(EXIT_FAILURE);
  }
  
  for (int i = 0; i < 10000; i++) {
    function_main(argc, argv);
  }
  
  LDS_LOG_INFO("test is end.","");
  while (1) {
    sleep(1);
  }

  return 0;
}


/********************* (C) COPYRIGHT SHENZHEN LDROBOT CO., LTD *******END OF
 * FILE ********/
