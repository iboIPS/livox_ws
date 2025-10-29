# Base image with pinned digest
FROM ros:jazzy@sha256:8cef3fe8344e738ad45b68b7d6f3a382c0850be776a7605ad81d269951ac019c

# Environment setup
ENV DEBIAN_FRONTEND=noninteractive \
    ROS_DISTRO=jazzy \
    ROS_ROOT=/opt/ros/jazzy \
    LD_LIBRARY_PATH=/Livox-SDK2/build/sdk_core:$LD_LIBRARY_PATH

# Install dependencies and ROS apt source updater
RUN set -eux; \
    apt-get update && \
    apt-get install -y --no-install-recommends \
        curl gnupg git cmake build-essential ca-certificates && \
    VERSION_CODENAME="$(. /etc/os-release && echo $VERSION_CODENAME)"; \
    ROS_APT_SOURCE_VERSION="$(curl -s https://api.github.com/repos/ros-infrastructure/ros-apt-source/releases/latest | grep -F tag_name | awk -F\" '{print $4}')"; \
    curl -L -o /tmp/ros2-apt-source.deb "https://github.com/ros-infrastructure/ros-apt-source/releases/download/${ROS_APT_SOURCE_VERSION}/ros2-apt-source_${ROS_APT_SOURCE_VERSION}.${VERSION_CODENAME}_all.deb"; \
    dpkg -i /tmp/ros2-apt-source.deb; \
    rm -rf /tmp/ros2-apt-source.deb; \
    apt-get clean && rm -rf /var/lib/apt/lists/*

# Build Livox-SDK2
WORKDIR /Livox-SDK2
# RUN git clone https://github.com/Livox-SDK/Livox-SDK2.git . && \
#     mkdir build && cd build && \
#     cmake .. && \
#     make -j$(nproc --ignore=2 || echo 1) && \
#     make install && \
#     cd sdk_core && chmod +r liblivox_lidar_sdk_shared.so

RUN apt-get update && apt-get install -y \
    curl gnupg git cmake build-essential ca-certificates \
    # System libraries
    libpcl-dev \
    rapidjson-dev \
    # ROS2 dependencies
    ros-${ROS_DISTRO}-rmw-cyclonedds-cpp \
    ros-${ROS_DISTRO}-rclcpp \
    ros-${ROS_DISTRO}-rclcpp-components \
    ros-${ROS_DISTRO}-std-msgs \
    ros-${ROS_DISTRO}-sensor-msgs \
    ros-${ROS_DISTRO}-rcutils \
    ros-${ROS_DISTRO}-pcl-conversions \
    ros-${ROS_DISTRO}-tf2-ros \
    ros-${ROS_DISTRO}-tf2-msgs \
    ros-${ROS_DISTRO}-pcl-conversions \
    ros-${ROS_DISTRO}-pcl-ros \
    && rm -rf /var/lib/apt/lists/*

# tmp for devel
RUN apt-get update && apt-get install -y \
        ros-${ROS_DISTRO}-rqt \
        ros-${ROS_DISTRO}-rviz2

# Build livox_ros_driver2
WORKDIR /livox_ws
RUN mkdir -p src/livox
COPY ./livox_ws/ /livox_ws/

# Source ROS environment for all future sessions
RUN echo "source /opt/ros/${ROS_DISTRO}/setup.bash" >> /root/.bashrc
RUN echo "export RMW_IMPLEMENTATION=rmw_cyclonedds_cpp" >> /root/.bashrc
RUN echo "source /usr/share/colcon_argcomplete/hook/colcon-argcomplete.bash" >> /root/.bashrc

# Default working directory
WORKDIR /livox_ws
ENTRYPOINT ["/bin/bash", "-c", "source /root/.bashrc && exec bash"]
