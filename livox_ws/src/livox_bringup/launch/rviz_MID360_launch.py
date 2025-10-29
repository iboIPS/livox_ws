import os
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument, LogInfo, TimerAction
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node
from ament_index_python.packages import get_package_share_directory


def generate_launch_description():
    pkg_share_dir = get_package_share_directory('livox_bringup')
    config_dir = os.path.join(pkg_share_dir, 'config')
    rviz_dir = os.path.join(pkg_share_dir, 'rviz')

    rviz_config_path = os.path.join(rviz_dir, 'display_point_cloud_ROS2.rviz')
    default_config_path = os.path.join(config_dir, 'MID360_config.json')

    # --- Declare first, then use LaunchConfiguration later ---
    declare_config_file = DeclareLaunchArgument(
        'user_config_path', default_value=default_config_path,
        description='Path to Livox LiDAR configuration JSON file'
    )

    user_config_path = LaunchConfiguration('user_config_path')

    livox_ros2_params = [
        {"user_config_path": user_config_path}
    ]

    livox_driver_node = Node(
        package='livox_node',
        executable='livox_node_node',
        name='livox_lidar_publisher',
        output='screen',
        emulate_tty=True,
        respawn=False,
        parameters=livox_ros2_params
    )

    rviz_node = Node(
        package='rviz2',
        executable='rviz2',
        name='rviz2_livox_view',
        output='screen',
        arguments=['--display-config', rviz_config_path]
    )

    # Start RViz after 3 seconds
    rviz_start = TimerAction(period=3.0, actions=[rviz_node])

    return LaunchDescription([
        declare_config_file,
        LogInfo(msg=['Using config: ', user_config_path]),
        livox_driver_node,
        rviz_start
    ])
