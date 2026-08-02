#include <chrono>
#include <memory>
#include <string>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>

#include <rclcpp/rclcpp.hpp>
#include "motor_control/msg/joystick.hpp"
#include <functional>

static const char* dev_name = "/dev/i2c-1";

// デバイスアドレス
#define MOTOR_LEFT 0x60
#define MOTOR_RIGHT 0x68

// レジスタアドレス
#define REG_ADDR_CONTROL 0x00

// 書き込み可能な電圧の最小値・最大値
#define VSET_MIN 0x06
#define VSET_MAX 0x3F

// 出力状態
#define RSET_FORWARD 0x01
#define RSET_REVERSE 0x02
#define RSET_STOP 0x00

#define DEADZONE 0.05 /* フロントエンド側でも設定しているが、念のため */


namespace motor_control_node
{

class MotorDataSubscriber : public rclcpp::Node
{
public:
  MotorDataSubscriber(const rclcpp::NodeOptions & options);
  ~MotorDataSubscriber();

private:
  int8_t makeMotorWriteData(float cmd);
  int8_t i2cWrite(uint8_t dev_addr, uint8_t reg_addr, uint8_t* data, uint16_t data_length);
  int32_t motoWriteData(float x, float y);
  void motorDataCallback(const motor_control::msg::Joystick::SharedPtr msg);
  rclcpp::Subscription<motor_control::msg::Joystick>::SharedPtr subscription_;
};

MotorDataSubscriber::MotorDataSubscriber(const rclcpp::NodeOptions & options) : Node("motor_data_subscriber", options)
{
  subscription_ = this->create_subscription<motor_control::msg::Joystick>("motor_data", 10, std::bind(&MotorDataSubscriber::motorDataCallback, this, std::placeholders::_1));
}

MotorDataSubscriber::~MotorDataSubscriber()
{
}

/*! 書き込みデータを生成する.
 * @param[in] cmd 要求速度
 * @param[out] 書き込みデータ
 */
int8_t MotorDataSubscriber::makeMotorWriteData(float cmd)
{
  uint8_t motor_voltage = 0x00;
  uint8_t motor_rotation = RSET_STOP;

  if(std::abs(cmd) > DEADZONE){
    motor_voltage = std::round(VSET_MIN + std::abs(cmd) * (VSET_MAX - VSET_MIN));
    motor_rotation = (cmd > 0) ? RSET_FORWARD : RSET_REVERSE;
  }
  
  RCLCPP_INFO(this->get_logger(), "電圧 = %x, 回転 = %x", motor_voltage, motor_rotation);
  motor_voltage =  motor_voltage << 2;
  return motor_voltage | motor_rotation;
}

/** I2Cスレーブデバイスにデータを書き込む.
 * @param[in] dev_addr デバイスアドレス.
 * @param[in] reg_addr レジスタアドレス.
 * @param[in] data 書き込むデータの格納場所を指すポインタ.
 * @param[in] data_length 書き込むデータの長さ.
 */
int8_t MotorDataSubscriber::i2cWrite(uint8_t dev_addr, uint8_t reg_addr, uint8_t* data, uint16_t data_length)
{
  /* I2Cデバイスをオープンする. */
  int32_t fd = open(dev_name, O_RDWR);

  if (fd == -1) {
    RCLCPP_FATAL(this->get_logger(), "i2cWrite: failed to open: %s\n", strerror(errno));
    return -1;
  }

  /* I2C-Write用のバッファを準備する.  */
  uint8_t* buffer = (uint8_t*)malloc(data_length + 1);
  if (buffer == NULL) {
    RCLCPP_FATAL(this->get_logger(), "i2cWrite: failed to memory allocate\n");
    close(fd);
    return -1;
  }
  buffer[0] = reg_addr;              /* 1バイト目にレジスタアドレスをセット. */
  memcpy(&buffer[1], data, data_length); /* 2バイト目以降にデータをセット. */ 

  /* I2C-Writeメッセージを作成する. */
  i2c_msg message[1];
  message[0].addr = dev_addr; /* addrは16bit幅 */
  message[0].flags = 0; /* read、writeやアドレス長の指定に利用 */
  message[0].len = data_length + 1; /* bufに指定するdataのサイズ */
  message[0].buf = buffer;

  struct i2c_rdwr_ioctl_data packets;
  packets.msgs = message;
  packets.nmsgs = 1;

  /* I2C-Writeを行う. */
  if (ioctl(fd, I2C_RDWR, &packets) != 1) {
    RCLCPP_FATAL(this->get_logger(), "i2cWrite: failed to ioctl: %s\n", strerror(errno));
    free(buffer);
    close(fd);
    return -1;
  }

  free(buffer);
  close(fd);
  return 0;
};

int32_t MotorDataSubscriber::motoWriteData(float x, float y)
{
  uint8_t leftWriteData[] = {0x00};
  uint8_t rightWriteData[] = {0x00};
  
  // 差動駆動を行う
  float left_cmd = std::max((float)-1.0, std::min((float)1.0, y + x)); // 左モーターの要求速度
  float right_cmd = std::max((float)-1.0, std::min((float)1.0, y - x)); // 右モーターの要求速度

  RCLCPP_INFO(this->get_logger(), "左モーター");
  leftWriteData[0] = makeMotorWriteData(left_cmd);
  RCLCPP_INFO(this->get_logger(), "右モーター");
  rightWriteData[0] = makeMotorWriteData((float)-1.0 * right_cmd); // 車体に取り付けた際に望みどおりに動くように、-1を掛けている

  i2cWrite(MOTOR_LEFT, REG_ADDR_CONTROL, leftWriteData, sizeof(leftWriteData));
  i2cWrite(MOTOR_RIGHT, REG_ADDR_CONTROL, rightWriteData, sizeof(rightWriteData));

  return 0;
}

void MotorDataSubscriber::motorDataCallback(const motor_control::msg::Joystick::SharedPtr msg)
{
  float x = msg->x;
  float y = msg->y;

  RCLCPP_INFO(this->get_logger(), "x=%f, y=%f", x, y);
  motoWriteData(x, y);
}

}

#include <rclcpp_components/register_node_macro.hpp>
RCLCPP_COMPONENTS_REGISTER_NODE(motor_control_node::MotorDataSubscriber)