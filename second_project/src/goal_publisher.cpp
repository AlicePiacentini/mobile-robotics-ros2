#include <chrono>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <memory>
#include <cmath>

#include "rclcpp/rclcpp.hpp"
#include "rclcpp_action/rclcpp_action.hpp"
#include "nav2_msgs/action/navigate_to_pose.hpp"
#include "geometry_msgs/msg/pose_stamped.hpp"
#include "ament_index_cpp/get_package_share_directory.hpp"

using namespace std::chrono_literals;

class GoalPublisher : public rclcpp::Node
{
public:

  using NavigateToPose = nav2_msgs::action::NavigateToPose;
  using GoalHandleNavigate =
    rclcpp_action::ClientGoalHandle<NavigateToPose>;

  struct GoalData
  {
    double x;
    double y;
    double theta;
  };

  GoalPublisher()
  : Node("goal_publisher"),
    current_goal_index_(0)
  {
    action_client_ =
      rclcpp_action::create_client<NavigateToPose>(
        this,
        "navigate_to_pose"
      );

    loadGoals();

    timer_ = this->create_wall_timer(
      2s,
      std::bind(&GoalPublisher::startNavigation, this)
    );
  }

private:

  rclcpp_action::Client<NavigateToPose>::SharedPtr action_client_;

  rclcpp::TimerBase::SharedPtr timer_;

  std::vector<GoalData> goals_;

  size_t current_goal_index_;

  bool started_ = false;

  void loadGoals()
  {
    std::string package_path =
      ament_index_cpp::get_package_share_directory(
        "second_project"
      );

    std::string csv_path =
      package_path + "/csv/goals.csv";

    std::ifstream file(csv_path);

    if (!file.is_open()) {
      RCLCPP_ERROR(
        this->get_logger(),
        "Cannot open CSV file"
      );
      return;
    }

    std::string line;

    std::getline(file, line);

    while (std::getline(file, line)) {

      if (line.empty()) {
        continue;
      }

      std::stringstream ss(line);

      std::string x_str;
      std::string y_str;
      std::string theta_str;

      std::getline(ss, x_str, ',');
      std::getline(ss, y_str, ',');
      std::getline(ss, theta_str, ',');

      GoalData goal;

      goal.x = std::stod(x_str);
      goal.y = std::stod(y_str);
      goal.theta = std::stod(theta_str);

      goals_.push_back(goal);
    }

    RCLCPP_INFO(
      this->get_logger(),
      "Loaded %zu goals",
      goals_.size()
    );
  }

  void startNavigation()
  {
    if (started_) {
      return;
    }

    started_ = true;

    timer_->cancel();

    if (!action_client_->wait_for_action_server(10s)) {

      RCLCPP_ERROR(
        this->get_logger(),
        "Nav2 action server not available"
      );

      return;
    }

    sendNextGoal();
  }

  void sendNextGoal()
  {
    if (current_goal_index_ >= goals_.size()) {

      RCLCPP_INFO(
        this->get_logger(),
        "All goals completed"
      );

      rclcpp::shutdown();

      return;
    }

    GoalData goal_data =
      goals_[current_goal_index_];

    NavigateToPose::Goal goal_msg;

    goal_msg.pose.header.frame_id = "map";

    goal_msg.pose.header.stamp = this->now();

    goal_msg.pose.pose.position.x =
      goal_data.x;

    goal_msg.pose.pose.position.y =
      goal_data.y;

    goal_msg.pose.pose.position.z = 0.0;

    goal_msg.pose.pose.orientation =
      yawToQuaternion(goal_data.theta);

    RCLCPP_INFO(
      this->get_logger(),
      "Sending goal %zu",
      current_goal_index_ + 1
    );

    auto send_goal_options =
      rclcpp_action::Client<NavigateToPose>::
      SendGoalOptions();

    send_goal_options.goal_response_callback =
      std::bind(
        &GoalPublisher::goalResponseCallback,
        this,
        std::placeholders::_1
      );

    send_goal_options.result_callback =
      std::bind(
        &GoalPublisher::resultCallback,
        this,
        std::placeholders::_1
      );

    action_client_->async_send_goal(
      goal_msg,
      send_goal_options
    );
  }

  geometry_msgs::msg::Quaternion
  yawToQuaternion(double yaw)
  {
    geometry_msgs::msg::Quaternion q;

    q.x = 0.0;
    q.y = 0.0;

    q.z = std::sin(yaw / 2.0);

    q.w = std::cos(yaw / 2.0);

    return q;
  }

  void goalResponseCallback(
    const GoalHandleNavigate::SharedPtr &
    goal_handle
  )
  {
    if (!goal_handle) {

      RCLCPP_ERROR(
        this->get_logger(),
        "Goal rejected"
      );

      current_goal_index_++;

      sendNextGoal();

      return;
    }

    RCLCPP_INFO(
      this->get_logger(),
      "Goal accepted"
    );
  }

  void resultCallback(
    const GoalHandleNavigate::WrappedResult &
    result
  )
  {
    if (
      result.code ==
      rclcpp_action::ResultCode::SUCCEEDED
    ) {

      RCLCPP_INFO(
        this->get_logger(),
        "Goal reached"
      );

    } else {

      RCLCPP_WARN(
        this->get_logger(),
        "Goal failed or aborted"
      );
    }

    current_goal_index_++;

    sendNextGoal();
  }
};

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);

  auto node =
    std::make_shared<GoalPublisher>();

  rclcpp::spin(node);

  rclcpp::shutdown();

  return 0;
}
