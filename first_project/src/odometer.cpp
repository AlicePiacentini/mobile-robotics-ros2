#include <cmath>
#include <rclcpp/rclcpp.hpp>
#include <nav_msgs/msg/odometry.hpp>
#include <bunker_msgs/msg/bunker_status.hpp>
#include <first_project/srv/reset_odom.hpp> 
#include <tf2_ros/transform_broadcaster.h>
#include <geometry_msgs/msg/transform_stamped.hpp>
#include <tf2/LinearMath/Quaternion.h> // AGGIUNTO: Necessario per calcolare gli angoli 3D

class OdometerNode : public rclcpp::Node {
public:
    OdometerNode() : Node("odometer"), x_(0.0), y_(0.0), theta_(0.0) {
        
        // 1. PUBLISHER: /project_odom
        odom_pub_ = this->create_publisher<nav_msgs::msg::Odometry>("/project_odom", 10);
        
        // 2. SUBSCRIBER: /bunker_status
        status_sub_ = this->create_subscription<bunker_msgs::msg::BunkerStatus>(
            "/bunker_status", 10, std::bind(&OdometerNode::status_callback, this, std::placeholders::_1));
            
        // 3. SERVICE: reset
        reset_srv_ = this->create_service<first_project::srv::ResetOdom>(
            "reset", std::bind(&OdometerNode::handle_reset, this, std::placeholders::_1, std::placeholders::_2));

        // 4. TF BROADCASTER: odom -> base_link2
        tf_broadcaster_ = std::make_unique<tf2_ros::TransformBroadcaster>(*this);

        last_time_ = this->now();
        RCLCPP_INFO(this->get_logger(), "Odometer avviato! In attesa dei dati dal robot...");
    }

private:
    void status_callback(const bunker_msgs::msg::BunkerStatus::SharedPtr msg) {
        
        rclcpp::Time current_time = this->now();
        double dt = (current_time - last_time_).seconds();

       
        if (dt > 1.0) {
            last_time_ = current_time;
            return;
        }

       
        double v = msg->linear_velocity;
        double w = msg->angular_velocity;

        
        x_ += v * cos(theta_) * dt;
        y_ += v * sin(theta_) * dt;
        theta_ += w * dt;

        
        auto odom_msg = nav_msgs::msg::Odometry();
        odom_msg.header.stamp = current_time;
        odom_msg.header.frame_id = "odom";
        odom_msg.child_frame_id = "base_link2";

        odom_msg.pose.pose.position.x = x_;
        odom_msg.pose.pose.position.y = y_;
        odom_msg.pose.pose.position.z = 0.0;

        tf2::Quaternion q;
        q.setRPY(0, 0, theta_); 
        odom_msg.pose.pose.orientation.x = q.x();
        odom_msg.pose.pose.orientation.y = q.y();
        odom_msg.pose.pose.orientation.z = q.z();
        odom_msg.pose.pose.orientation.w = q.w();

        odom_msg.twist.twist.linear.x = v;
        odom_msg.twist.twist.angular.z = w;

        odom_pub_->publish(odom_msg);

        
        geometry_msgs::msg::TransformStamped t;
        t.header.stamp = current_time;
        t.header.frame_id = "odom";
        t.child_frame_id = "base_link2";

        t.transform.translation.x = x_;
        t.transform.translation.y = y_;
        t.transform.translation.z = 0.0;
        
        t.transform.rotation.x = q.x();
        t.transform.rotation.y = q.y();
        t.transform.rotation.z = q.z();
        t.transform.rotation.w = q.w();

        tf_broadcaster_->sendTransform(t);

        
        last_time_ = current_time;
    }

    void handle_reset(const std::shared_ptr<first_project::srv::ResetOdom::Request> request, std::shared_ptr<first_project::srv::ResetOdom::Response> response) {
        (void)request;
        x_ = 0.0;
        y_ = 0.0;
        theta_ = 0.0;

	rclcpp::Time current_time = this->now();
        last_time_ = current_time;

	nav_msgs::msg::Odometry odom_msg;
        odom_msg.header.stamp = current_time;
        odom_msg.header.frame_id = "odom";
        odom_msg.child_frame_id = "base_link2";
    
        odom_msg.pose.pose.position.x = 0.0;
    	odom_msg.pose.pose.position.y = 0.0;
    	odom_msg.pose.pose.position.z = 0.0;
    	
	odom_msg.pose.pose.orientation.x = 0.0;
    	odom_msg.pose.pose.orientation.y = 0.0;
    	odom_msg.pose.pose.orientation.z = 0.0;
    	odom_msg.pose.pose.orientation.w = 1.0;
    	
	odom_msg.twist.twist.linear.y = 0.0;
	odom_msg.twist.twist.linear.z = 0.0;
	odom_msg.twist.twist.angular.x = 0.0;
	odom_msg.twist.twist.angular.y = 0.0;
	odom_msg.twist.twist.linear.x = 0.0;
    	odom_msg.twist.twist.angular.z = 0.0;
    	
	odom_pub_->publish(odom_msg);
    
	geometry_msgs::msg::TransformStamped t;
    	t.header.stamp = current_time;
    	t.header.frame_id = "odom";
    	t.child_frame_id = "base_link2";
    	
	t.transform.translation.x = 0.0;
    	t.transform.translation.y = 0.0;
    	t.transform.translation.z = 0.0;
    	
	t.transform.rotation.x = 0.0;
    	t.transform.rotation.y = 0.0;
    	t.transform.rotation.z = 0.0;
    	t.transform.rotation.w = 1.0;
    
	tf_broadcaster_->sendTransform(t);
        
	RCLCPP_INFO(this->get_logger(), "RESET CHIAMATO: Odometria ripristinata a (0,0,0)!");
        
	response->success = true;
         
    }

    
    double x_, y_, theta_;
    rclcpp::Time last_time_; 

    rclcpp::Publisher<nav_msgs::msg::Odometry>::SharedPtr odom_pub_;
    rclcpp::Subscription<bunker_msgs::msg::BunkerStatus>::SharedPtr status_sub_;
    rclcpp::Service<first_project::srv::ResetOdom>::SharedPtr reset_srv_;
    std::unique_ptr<tf2_ros::TransformBroadcaster> tf_broadcaster_;
};

int main(int argc, char ** argv) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<OdometerNode>());
    rclcpp::shutdown();
    return 0;
}
