cmake_minimum_required(VERSION 3.8)
project(first_project)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(nav_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(tf2_ros REQUIRED)
find_package(tf2 REQUIRED)
find_package(std_msgs REQUIRED)
find_package(rosidl_default_generators REQUIRED)
find_package(rosidl_default_runtime REQUIRED)
find_package(bunker_msgs REQUIRED)

# Custom interfaces
rosidl_generate_interfaces(${PROJECT_NAME}
  "msg/TfErrorMsg.msg"
  "srv/ResetOdom.srv"
  DEPENDENCIES std_msgs nav_msgs geometry_msgs
)

# --- NODO 1: odometer ---
add_executable(odometer src/odometer.cpp)
ament_target_dependencies(odometer rclcpp nav_msgs geometry_msgs tf2_ros tf2 bunker_msgs)



# --- NODO 2: tf_error ---
add_executable(tf_error src/tf_error.cpp)
ament_target_dependencies(tf_error rclcpp geometry_msgs tf2_ros)


rosidl_get_typesupport_target(cpp_typesupport_target ${PROJECT_NAME} "rosidl_typesupport_cpp")


target_link_libraries(odometer "${cpp_typesupport_target}")
target_link_libraries(tf_error "${cpp_typesupport_target}")

install(TARGETS
  odometer
  tf_error
  DESTINATION lib/${PROJECT_NAME}
)

install(DIRECTORY
  launch
  rviz
  DESTINATION share/${PROJECT_NAME}
)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  set(ament_cmake_copyright_FOUND TRUE)
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

ament_export_dependencies(rosidl_default_runtime)

ament_package()
