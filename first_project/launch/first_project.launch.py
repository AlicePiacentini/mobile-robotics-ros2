from launch import LaunchDescription
from launch_ros.actions import Node
from ament_index_python.packages import get_package_share_directory
import os


def generate_launch_description():
    pkg_share = get_package_share_directory('first_project')
    rviz_config = os.path.join(pkg_share, 'rviz', 'first_project.rviz')

    return LaunchDescription([
        Node(
            package='first_project',
            executable='odometer',
            name='odometer',
            output='screen'
        ),
        Node(
            package='first_project',
            executable='tf_error',
            name='tf_error',
            output='screen'
        ),
        Node(
            package='rviz2',
            executable='rviz2',
            name='rviz2',
            output='screen',
            arguments=['-d', rviz_config]
        )
    ])
