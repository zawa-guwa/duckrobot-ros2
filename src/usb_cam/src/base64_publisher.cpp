#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/image.hpp>
#include <std_msgs/msg/string.hpp>
#include <cv_bridge/cv_bridge.hpp>
#include <opencv2/opencv.hpp>
#include <sstream>
#include <vector>

namespace usb_cam_node
{

// base64エンコード用の簡易関数
static const std::string base64_chars =
	"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
	"abcdefghijklmnopqrstuvwxyz"
	"0123456789+/";

std::string base64_encode(const unsigned char* buf, unsigned int bufLen) {
	std::string ret;
	int i = 0;
	int j = 0;
	unsigned char char_array_3[3];
	unsigned char char_array_4[4];

	while (bufLen--) {
			char_array_3[i++] = *(buf++);
			if (i == 3) {
					char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
					char_array_4[1] = ((char_array_3[0] & 0x03) << 4) +
														((char_array_3[1] & 0xf0) >> 4);
					char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) +
														((char_array_3[2] & 0xc0) >> 6);
					char_array_4[3] = char_array_3[2] & 0x3f;

					for(i = 0; (i <4) ; i++)
							ret += base64_chars[char_array_4[i]];
					i = 0;
			}
	}

	if (i) {
			for(j = i; j < 3; j++)
					char_array_3[j] = '\0';

			char_array_4[0] = ( char_array_3[0] & 0xfc ) >> 2;
			char_array_4[1] = ( ( char_array_3[0] & 0x03 ) << 4 ) +
												( ( char_array_3[1] & 0xf0 ) >> 4 );
			char_array_4[2] = ( ( char_array_3[1] & 0x0f ) << 2 ) +
												( ( char_array_3[2] & 0xc0 ) >> 6 );
			char_array_4[3] = char_array_3[2] & 0x3f;

			for (j = 0; (j < i + 1); j++)
					ret += base64_chars[char_array_4[j]];

			while((i++ < 3))
					ret += '=';
	}

	return ret;
}

// class Base64Publisher : public rclcpp::Node {
// public:
//   Base64Publisher() : Node("image_base64_publisher") {
//     using std::placeholders::_1;
//     sub_ = this->create_subscription<sensor_msgs::msg::Image>(
//       "image_raw", 10, std::bind(&Base64Publisher::callback, this, _1));
//     pub_ = this->create_publisher<std_msgs::msg::String>("image_base64", 10);
//   }

// private:
//   void callback(const sensor_msgs::msg::Image::SharedPtr msg) {
//     auto cv_img = cv_bridge::toCvCopy(msg, "bgr8");
//     std::vector<uchar> buf;
//     cv::imencode(".jpg", cv_img->image, buf);

//     std::string encoded = base64_encode(buf.data(), buf.size());

//     std_msgs::msg::String out;
//     out.data = encoded;
//     pub_->publish(out);
//   }

//   rclcpp::Subscription<sensor_msgs::msg::Image>::SharedPtr sub_;
//   rclcpp::Publisher<std_msgs::msg::String>::SharedPtr pub_;
// };


class Base64Publisher : public rclcpp::Node {
public:
  Base64Publisher(rclcpp::NodeOptions options);
  ~Base64Publisher();

private:
  void publishBase64(const sensor_msgs::msg::Image::SharedPtr msg);
  rclcpp::Subscription<sensor_msgs::msg::Image>::SharedPtr sub_;
  rclcpp::Publisher<std_msgs::msg::String>::SharedPtr pub_;
};

Base64Publisher::Base64Publisher(rclcpp::NodeOptions options) : Node("image_base64_publisher", options)
{
	using std::placeholders::_1;
	sub_ = this->create_subscription<sensor_msgs::msg::Image>(
		"image_raw", 10, std::bind(&Base64Publisher::publishBase64, this, _1));
	pub_ = this->create_publisher<std_msgs::msg::String>("image_base64", 10);
}

Base64Publisher::~Base64Publisher()
{
}

void Base64Publisher::publishBase64(const sensor_msgs::msg::Image::SharedPtr msg)
{
	auto cv_img = cv_bridge::toCvCopy(msg, "bgr8");
	std::vector<uchar> buf;
	cv::imencode(".jpg", cv_img->image, buf);

	std::string encoded = base64_encode(buf.data(), buf.size());

	std_msgs::msg::String out;
	out.data = encoded;
	pub_->publish(out);
}
}

#include <rclcpp_components/register_node_macro.hpp>
RCLCPP_COMPONENTS_REGISTER_NODE(usb_cam_node::Base64Publisher)