#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/image.hpp>
#include <opencv2/opencv.hpp>
#include "cv_bridge/cv_bridge.hpp"

namespace usb_cam_node
{

class ImagePublisher : public rclcpp::Node {
public:
  ImagePublisher(rclcpp::NodeOptions options);
  ~ImagePublisher();

private:
  void publishImage();
  rclcpp::TimerBase::SharedPtr timer_;
  cv::VideoCapture camera_;
  rclcpp::Publisher<sensor_msgs::msg::Image>::SharedPtr image_pub_;
};


ImagePublisher::ImagePublisher(rclcpp::NodeOptions options) : Node("image_publisher", options)
{
  timer_ = this->create_wall_timer(std::chrono::milliseconds(1000 / 30), std::bind(&ImagePublisher::publishImage, this));
  image_pub_ = create_publisher<sensor_msgs::msg::Image>("image_raw", 10);

  auto camera_path = this->declare_parameter<std::string>("camera_path", "/dev/video0");
  camera_.open(camera_path);
  if(!camera_.isOpened()){
    RCLCPP_ERROR(this->get_logger(), "Failed to open %s", camera_path.c_str());
    rclcpp::shutdown();
  }
}

ImagePublisher::~ImagePublisher()
{
}

void ImagePublisher::publishImage()
{
  sensor_msgs::msg::Image ros_img;
  cv_bridge::CvImage cv_img;
  cv_img.encoding = "bgr8";

  camera_ >> cv_img.image;
  cv_img.header.stamp = this->now();
  cv_img.toImageMsg(ros_img);
  image_pub_->publish(std::move(ros_img));
}

}

#include <rclcpp_components/register_node_macro.hpp>
RCLCPP_COMPONENTS_REGISTER_NODE(usb_cam_node::ImagePublisher)

