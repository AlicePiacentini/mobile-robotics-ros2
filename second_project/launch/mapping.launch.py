from launch import LaunchDescription
from launch_ros.actions import Node


def generate_launch_description():

    return LaunchDescription([

        Node(
            package='pointcloud_to_laserscan',
            executable='pointcloud_to_laserscan_node',
            name='pointcloud_to_laserscan',
            output='screen',
            remappings=[
                ('cloud_in', '/ugv/rslidar_points'),
                ('scan', '/scan')
            ],
            parameters=[{
                'target_frame': 'base_link',
                'transform_tolerance': 0.01,
                'min_height': -0.5,
                'max_height': 1.0,
                'angle_min': -3.14,
                'angle_max': 3.14,
                'angle_increment': 0.0087,
                'scan_time': 0.1,
                'range_min': 0.2,
                'range_max': 30.0,
                'use_inf': True,
                'inf_epsilon': 1.0
            }]
        ),

        Node(
            package='slam_toolbox',
            executable='async_slam_toolbox_node',
            name='slam_toolbox',
            output='screen',
            parameters=[{
                'use_sim_time': True,
                'odom_frame': 'odom',
                'map_frame': 'map',
                'base_frame': 'UGV_base_link',
                'scan_topic': '/scan'
            }]
        ),

	# Node(
	#     package='rviz2',
	#     executable='rviz2',
	#     name='rviz2',
	#     output='screen'
	# )
    ])

