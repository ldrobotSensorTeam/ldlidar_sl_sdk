cmake_minimum_required(VERSION 2.8.3)
project(ldlidar_sl)

if(${CMAKE_BUILD_TYPE} MATCHES  "Release")
  #set(CMAKE_BUILD_TYPE "Release") 
  set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -std=c++11 -Wall")
  #set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -std=c++11 -O3 -Wall")
  message(STATUS "Mode: Release")
  message(STATUS "optional:-std=c++11 -Wall")
else()
  #set(CMAKE_BUILD_TYPE "Debug") 
  set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -std=c++11 -Wall -Wextra -Wpedantic -g2 -ggdb")
  #set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -std=c++11 -O2 -Wall -Wextra -Wpedantic -g2 -ggdb")
  message(STATUS "Mode: Debug")
  message(STATUS "optional:-std=c++11 -Wall -Wextra -Wpedantic -g2 -ggdb")
endif()

###########
## Build ##
###########

## Specify additional locations of header files
## Your package locations should be listed before other locations
include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/../../ldlidar_driver/include
)

file(GLOB  LDLIDAR_DRI ${CMAKE_CURRENT_SOURCE_DIR}/../../ldlidar_driver/src/*.cpp)

add_executable(${PROJECT_NAME}_node ${CMAKE_CURRENT_SOURCE_DIR}/test.cpp)

add_library(ldlidar_driver_static STATIC 
  ${LDLIDAR_DRI} 
)

add_library(ldlidar_driver_shared SHARED 
  ${LDLIDAR_DRI} 
)

# rename library name
set_target_properties (ldlidar_driver_static PROPERTIES OUTPUT_NAME "ldlidar_driver")
set_target_properties (ldlidar_driver_shared PROPERTIES OUTPUT_NAME "ldlidar_driver")

# binary file link to library
target_link_libraries(${PROJECT_NAME}_node 
  ldlidar_driver_static 
  pthread 
  wiringPi
)

###########
## Install ##
###########

INSTALL(TARGETS ldlidar_driver_static ldlidar_driver_shared
  ARCHIVE DESTINATION lib/ldlidar_driver
  LIBRARY DESTINATION share/ldlidar_driver
)

INSTALL(DIRECTORY ${PROJECT_SOURCE_DIR}/ldlidar_driver/include
  DESTINATION include/ldlidar_driver
)
