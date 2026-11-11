cmake_minimum_required(VERSION 3.8)
project(second_project)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rclcpp_action REQUIRED)
find_package(nav2_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(ament_index_cpp REQUIRED)

add_executable(goal_publisher src/goal_publisher.cpp)

ament_target_dependencies(goal_publisher
  rclcpp
  rclcpp_action
  nav2_msgs
  geometry_msgs
  ament_index_cpp
)

install(TARGETS
  goal_publisher
  DESTINATION lib/${PROJECT_NAME}
)

install(DIRECTORY
  launch
  csv
  config
  DESTINATION share/${PROJECT_NAME}
)

ament_package()
