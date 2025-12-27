import launch
from launch_ros.actions import ComposableNodeContainer
from launch_ros.descriptions import ComposableNode


def generate_launch_description():
    container = ComposableNodeContainer(
            name='image_container',
            namespace='',
            package='rclcpp_components',
            executable='component_container',
            composable_node_descriptions=[
                ComposableNode(
                    package='usb_cam',
                    plugin='usb_cam_node::ImagePublisher',
                    name='image_publisher_node',
                    # extra_arguments=[{'use_intra_process_comms': True}]),
                    ),
                # ComposableNode(
                #     package='usb_cam',
                #     plugin='usb_cam_node::ImageSubscriber',
                #     name='image_subscriber_node',
                #     # extra_arguments=[{'use_intra_process_comms': True}]),
                #     ),
                ComposableNode(
                    package='usb_cam',
                    plugin='usb_cam_node::Base64Publisher',
                    name='base64_publisher_node',
                    # extra_arguments=[{'use_intra_process_comms': True}]),
                    )
            ],
            output='both',
    )

    return launch.LaunchDescription([container])