import launch
from launch_ros.actions import ComposableNodeContainer
from launch_ros.descriptions import ComposableNode


def generate_launch_description():
    container = ComposableNodeContainer(
        name='audio_output_container',
        namespace='',
        package='rclcpp_components',
        executable='component_container',
        composable_node_descriptions=[
            ComposableNode(
                package='speaker',
                plugin='audio_output_node::AudioStateSubscriber',
                parameters=[
                    {'video_path': './video/duck_voice_2000Hz_noisereduction.mov'}
                ],
            ),
        ],  
        output='both',
    )

    return launch.LaunchDescription([container])