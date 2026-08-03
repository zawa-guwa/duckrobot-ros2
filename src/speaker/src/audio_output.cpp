#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/bool.hpp>
#include <unistd.h>
#include <string>

namespace audio_output_node
{

class AudioStateSubscriber : public rclcpp::Node
{
public:
  AudioStateSubscriber(const rclcpp::NodeOptions & options);
  ~AudioStateSubscriber();

private:
  bool requested_state = false;
  bool current_state = false;
  void audioStateCallback(const std_msgs::msg::Bool::SharedPtr msg);
  void startPlay();
  void stopPlay();
  rclcpp::Subscription<std_msgs::msg::Bool>::SharedPtr subscription_;
};

AudioStateSubscriber::AudioStateSubscriber(const rclcpp::NodeOptions & options) : Node("audio_state_subscriber", options)
{
  subscription_ = this->create_subscription<std_msgs::msg::Bool>("audio_state", 10, std::bind(&AudioStateSubscriber::audioStateCallback, this, std::placeholders::_1));
  this->declare_parameter("video_path", "./video_dummy/duck_voice_2000Hz_noisereduction.mov"); 
}

AudioStateSubscriber::~AudioStateSubscriber()
{
}

void AudioStateSubscriber::startPlay()
{
  std::string video_path = this->get_parameter("video_path").as_string();
  std::string command = "ffplay -vn -nodisp -autoexit -loop 0 " + video_path + " &";
  
  system(command.c_str());
  
  current_state = true;
}

void AudioStateSubscriber::stopPlay()
{
  system("pkill ffplay");

  current_state = false;
}

void AudioStateSubscriber::audioStateCallback(const std_msgs::msg::Bool::SharedPtr msg)
{
  requested_state = msg->data;
  RCLCPP_INFO(this->get_logger(), "Received: %s", requested_state ? "true" : "false");
  
  if (requested_state && !current_state) { /* WebUIからの再生要求があり、現在再生中でなければ、音を再生 */
    startPlay();
  }

  if (!requested_state && current_state) { /* WebUIからの停止要求があり、現在再生中であれば、音を停止 */
    stopPlay();
  }
}

}

#include <rclcpp_components/register_node_macro.hpp>
RCLCPP_COMPONENTS_REGISTER_NODE(audio_output_node::AudioStateSubscriber)