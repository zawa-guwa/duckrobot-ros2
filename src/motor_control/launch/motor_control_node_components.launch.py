import launch
from launch_ros.actions import ComposableNodeContainer
from launch_ros.descriptions import ComposableNode


def generate_launch_description():
    container = ComposableNodeContainer(
        name='motor_control_container',
        namespace='',
        package='rclcpp_components',
        executable='component_container',
        composable_node_descriptions=[
            ComposableNode(
                package='motor_control',
                plugin='motor_control_node::MotorDataSubscriber',
                name='motor_data_subscriber_node',
            ),
        ],
        output='both',
    )

    return launch.LaunchDescription([container])