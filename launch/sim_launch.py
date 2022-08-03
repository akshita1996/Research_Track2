""".. module:: sim_launch
      :platform: Unix
      :synopsis: Python module for launching all my nodes 
      
      .. Author:: Anderson Siqueira
"""

import launch
from launch_ros.actions import ComposableNodeContainer
from launch_ros.descriptions import ComposableNode

def generate_launch_description():
    """Generate launch description with multiple components."""
    container = ComposableNodeContainer(
            name='my_container',
            namespace='',
            package='rclcpp_components',
            executable='component_container',
            composable_node_descriptions=[
                ComposableNode(
                    package='rt2_assignment1',
                    plugin='rt2_assignment1::Position_srv',
                    name='pos_node'),
                ComposableNode(
                    package='rt2_assignment1',
                    plugin='rt2_assignment1::State_Machine',
                    name='sm_node'),                
            ],
            output='screen',
    )

    return launch.LaunchDescription([container])

