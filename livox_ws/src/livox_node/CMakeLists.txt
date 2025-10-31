cmake_minimum_required(VERSION 3.8)
cmake_policy(SET CMP0144 NEW)
project(livox_node)
set(CMAKE_SUPPRESS_DEVELOPER_WARNINGS 1 CACHE INTERNAL "No dev warnings")

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rclcpp_components REQUIRED)
find_package(std_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(pcl_conversions REQUIRED)
find_package(pcl_ros REQUIRED)
find_package(livox_core REQUIRED)

add_library(${PROJECT_NAME} SHARED
  src/lddc.cpp
  src/driver_node.cpp
  src/livox_ros_driver2.cpp
)

# your own public includes
target_include_directories(${PROJECT_NAME}
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# link to the **exported** target from livox_core
target_link_libraries(${PROJECT_NAME}
  livox_core::livox_core
)

ament_target_dependencies(${PROJECT_NAME}
  rclcpp rclcpp_components
  std_msgs sensor_msgs
  pcl_conversions pcl_ros
)

rclcpp_components_register_node(${PROJECT_NAME}
  PLUGIN "livox_ros::DriverNode"
  EXECUTABLE ${PROJECT_NAME}_node
)

install(TARGETS
  ${PROJECT_NAME}
  ${PROJECT_NAME}_node
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION lib/${PROJECT_NAME}
)

install(DIRECTORY include/ DESTINATION include)
install(FILES package.xml DESTINATION share/${PROJECT_NAME})

ament_package()
