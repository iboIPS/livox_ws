cmake_minimum_required(VERSION 3.8)
cmake_policy(SET CMP0144 NEW)
project(livox_core)
set(CMAKE_SUPPRESS_DEVELOPER_WARNINGS 1 CACHE INTERNAL "No dev warnings")

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# ------------------------------------------------------------------------------
# Dependencies
# ------------------------------------------------------------------------------
find_package(ament_cmake REQUIRED)
find_package(PCL REQUIRED)
find_package(PkgConfig REQUIRED)
find_package(RapidJSON REQUIRED)
pkg_check_modules(APR apr-1)

# ------------------------------------------------------------------------------
# Livox SDK
# ------------------------------------------------------------------------------
find_path(LIVOX_LIDAR_SDK_INCLUDE_DIR
  NAMES livox_lidar_api.h livox_lidar_def.h
  PATHS /usr/local/include
  REQUIRED
)

find_library(LIVOX_LIDAR_SDK_LIBRARY
  NAMES livox_lidar_sdk_shared
  PATHS /usr/local/lib
  REQUIRED
)

# ------------------------------------------------------------------------------
# Includes
# ------------------------------------------------------------------------------
include_directories(
  include
  include/livox_core
  ${PCL_INCLUDE_DIRS}
  ${APR_INCLUDE_DIRS}
  ${LIVOX_LIDAR_SDK_INCLUDE_DIR}
)

# ------------------------------------------------------------------------------
# Library
# ------------------------------------------------------------------------------
add_library(${PROJECT_NAME} SHARED
  src/lds.cpp
  src/lds_lidar.cpp

  src/comm/comm.cpp
  src/comm/ldq.cpp
  src/comm/semaphore.cpp
  src/comm/lidar_imu_data_queue.cpp
  src/comm/cache_index.cpp
  src/comm/pub_handler.cpp

  src/parse_cfg_file/parse_cfg_file.cpp
  src/parse_cfg_file/parse_livox_lidar_cfg.cpp

  src/call_back/lidar_common_callback.cpp
  src/call_back/livox_lidar_callback.cpp
)

# public includes propagate to dependents
target_include_directories(${PROJECT_NAME}
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/livox_core>
    $<INSTALL_INTERFACE:include>
    ${LIVOX_LIDAR_SDK_INCLUDE_DIR}
    ${PCL_INCLUDE_DIRS}
    ${APR_INCLUDE_DIRS}
)

# link (SDK in PUBLIC so it propagates transitively)
target_link_libraries(${PROJECT_NAME}
  PUBLIC
    ${LIVOX_LIDAR_SDK_LIBRARY}
  PRIVATE
    ${PCL_LIBRARIES}
    ${APR_LIBRARIES}
)

# namespaced alias for consumers
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

# export + install
install(TARGETS ${PROJECT_NAME}
  EXPORT ${PROJECT_NAME}Targets
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin)

install(EXPORT ${PROJECT_NAME}Targets
  FILE ${PROJECT_NAME}Targets.cmake
  NAMESPACE ${PROJECT_NAME}::
  DESTINATION share/${PROJECT_NAME}/cmake)

install(DIRECTORY include/ DESTINATION include)
install(FILES package.xml DESTINATION share/${PROJECT_NAME})

ament_export_include_directories(include include/livox_core ${LIVOX_LIDAR_SDK_INCLUDE_DIR})
ament_export_libraries(${PROJECT_NAME})
ament_export_targets(${PROJECT_NAME}Targets HAS_LIBRARY_TARGET)

ament_package()
