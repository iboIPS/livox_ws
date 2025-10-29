import os
from launch import LaunchDescription
from launch.actions import LogInfo
from launch_ros.actions import ComposableNodeContainer
from launch_ros.descriptions import ComposableNode
from ament_index_python.packages import get_package_share_directory

def generate_launch_description():
    pkg_share_dir = get_package_share_directory('livox_bringup')
    config_dir = os.path.join(pkg_share_dir, 'config')
    rviz_dir = os.path.join(pkg_share_dir, 'rviz')

    rviz_config_path = os.path.join(rviz_dir, 'display_point_cloud_ROS2.rviz')
    user_config_path = os.path.join(config_dir, 'MID360_config.json')

    # -------------------------------------------------------------------------
    # Livox composable node definition
    # -------------------------------------------------------------------------
    livox_node = ComposableNode(
        package='livox_node',
        plugin='livox_ros::DriverNode',
        name='livox_lidar_publisher',
        parameters=[{
            'user_config_path': user_config_path,
        }]
    )

    # -------------------------------------------------------------------------
    # Container to host composable nodes
    # -------------------------------------------------------------------------
    container = ComposableNodeContainer(
        name='livox_container',
        namespace='',
        package='rclcpp_components',
        executable='component_container_mt',
        composable_node_descriptions=[livox_node],
        output='screen',
        emulate_tty=True,
        arguments=['--ros-args', '-p', 'use_intra_process_comms:=true']
    )

    # -------------------------------------------------------------------------
    # Optional RViz2 node in a separate process
    # -------------------------------------------------------------------------
    from launch_ros.actions import Node
    rviz_node = Node(
        package='rviz2',
        executable='rviz2',
        name='rviz2_livox_view',
        output='screen',
        arguments=['--display-config', rviz_config_path]
    )

    return LaunchDescription([
        LogInfo(msg=f'Using Livox config: {user_config_path}'),
        container,
        rviz_node
    ])
