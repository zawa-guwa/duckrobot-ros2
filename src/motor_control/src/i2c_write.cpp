#include <chrono>
#include <memory>
#include <string>

#include <rclcpp/rclcpp.hpp>
#include "motor_control/msg/joystick.hpp"
#include <functional>


namespace motor_control_node
{

class MotorDataSubscriber : public rclcpp::Node
{
public:
  MotorDataSubscriber(const rclcpp::NodeOptions & options);
  ~MotorDataSubscriber();

  // MotorDataSubscriber()
  // : Node("minimal_subscriber")
  // {
  //   auto topic_callback =
  //     [this](std_msgs::msg::String::UniquePtr msg) -> void {
  //       RCLCPP_INFO(this->get_logger(), "I heard: '%s'", msg->data.c_str());
  //     };
  //   subscription_ =
  //     this->create_subscription<std_msgs::msg::String>("topic", 10, topic_callback);
  // }

private:
  // void motorDataCallback(std_msgs::msg::String::UniquePtr msg);
  // rclcpp::Subscription<std_msgs::msg::String>::SharedPtr subscription_;
  void motorDataCallback(const motor_control::msg::Joystick::SharedPtr msg);
  rclcpp::Subscription<motor_control::msg::Joystick>::SharedPtr subscription_;
};

MotorDataSubscriber::MotorDataSubscriber(const rclcpp::NodeOptions & options) : Node("motor_data_subscriber", options)
{
	// using std::placeholders::_1;
	// subscription_ = this->create_subscription<std_msgs::msg::String>("motor_data", 10, std::bind(&MotorDataSubscriber::motorDataCallback, this, std::placeholders::_1));
  subscription_ = this->create_subscription<motor_control::msg::Joystick>("motor_data", 10, std::bind(&MotorDataSubscriber::motorDataCallback, this, std::placeholders::_1));
}

MotorDataSubscriber::~MotorDataSubscriber()
{
}

// void MotorDataSubscriber::motorDataCallback(std_msgs::msg::String::UniquePtr msg)
// {
//   RCLCPP_INFO(this->get_logger(), "I heard: '%s'", msg->data.c_str());
// }

void MotorDataSubscriber::motorDataCallback(const motor_control::msg::Joystick::SharedPtr msg)
{
  int x = msg->x;
  int y = msg->y;

  RCLCPP_INFO(this->get_logger(), "x=%d, y=%d", x, y);
}

}

#include <rclcpp_components/register_node_macro.hpp>
RCLCPP_COMPONENTS_REGISTER_NODE(motor_control_node::MotorDataSubscriber)