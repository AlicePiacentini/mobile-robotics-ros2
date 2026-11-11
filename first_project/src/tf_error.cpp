#include <chrono>
#include <cmath>
#include <memory>
#include <string>

#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/transform_stamped.hpp"
#include "tf2_ros/buffer.h"
#include "tf2_ros/transform_listener.h"
#include "tf2/exceptions.h"
#include "first_project/msg/tf_error_msg.hpp"

using namespace std::chrono_literals;

class TfError : public rclcpp::Node
{
public:
  TfError()
  : Node("tf_error"),
    tf_buffer_(this->get_clock()),
    tf_listener_(tf_buffer_),
    start_time_(this->now()),
    travelled_distance_(0.0),
    last_x_(0.0),
    last_y_(0.0),
    first_measure_(true)
  {
    error_publisher_ =
      this->create_publisher<first_project::msg::TfErrorMsg>("/tf_error_msg", 10);

    timer_ = this->create_wall_timer(
      100ms, std::bind(&TfError::update_error, this));

    RCLCPP_INFO(this->get_logger(), "tf_error node started");
  }

private:
  void update_error()
  {
    geometry_msgs::msg::TransformStamped real_tf;
    geometry_msgs::msg::TransformStamped estimated_tf;

    try {
      real_tf = tf_buffer_.lookupTransform("odom", "base_link", tf2::TimePointZero);
      estimated_tf = tf_buffer_.lookupTransform("odom", "base_link2", tf2::TimePointZero);
    } catch (const tf2::TransformException & ex) {
      RCLCPP_WARN_THROTTLE(
        this->get_logger(),
        *this->get_clock(),
        2000,
        "Transform not available yet: %s",
        ex.what());
      return;
    }

    double x_real = real_tf.transform.translation.x;
    double y_real = real_tf.transform.translation.y;

    double x_est = estimated_tf.transform.translation.x;
    double y_est = estimated_tf.transform.translation.y;

    double current_error = std::sqrt(
      (x_real - x_est) * (x_real - x_est) +
      (y_real - y_est) * (y_real - y_est));

    if (first_measure_) {
      last_x_ = x_est;
      last_y_ = y_est;
      first_measure_ = false;
    } else {
      double step_distance = std::sqrt(
        (x_est - last_x_) * (x_est - last_x_) +
        (y_est - last_y_) * (y_est - last_y_));
      travelled_distance_ += step_distance;

      last_x_ = x_est;
      last_y_ = y_est;
    }

    first_project::msg::TfErrorMsg msg;
    msg.header.stamp = this->now();
    msg.header.frame_id = "odom";
    msg.tf_error = static_cast<float>(current_error);
    msg.time_from_start =
      static_cast<int32_t>((this->now() - start_time_).seconds());
    msg.travelled_distance = static_cast<float>(travelled_distance_);

    error_publisher_->publish(msg);
  }

  rclcpp::Publisher<first_project::msg::TfErrorMsg>::SharedPtr error_publisher_;
  rclcpp::TimerBase::SharedPtr timer_;

  tf2_ros::Buffer tf_buffer_;
  tf2_ros::TransformListener tf_listener_;

  rclcpp::Time start_time_;

  double travelled_distance_;
  double last_x_;
  double last_y_;
  bool first_measure_;
};

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<TfError>());
  rclcpp::shutdown();
  return 0;
}
