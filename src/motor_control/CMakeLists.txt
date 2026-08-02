cmake_minimum_required(VERSION 3.8)
project(motor_control)

set(TARGET_NAME motor_control_component)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rclcpp_components REQUIRED)
find_package(rosidl_default_generators REQUIRED)

# install launch files
install(
  DIRECTORY launch
  DESTINATION share/${PROJECT_NAME}/
)

rosidl_generate_interfaces(${PROJECT_NAME}
  "msg/Joystick.msg"
)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  # the following line skips the linter which checks for copyrights
  # comment the line when a copyright and license is added to all source files
  set(ament_cmake_copyright_FOUND TRUE)
  # the following line skips cpplint (only works in a git repo)
  # comment the line when this package is in a git repo and when
  # a copyright and license is added to all source files
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

ament_export_dependencies(rosidl_default_runtime)

add_library(${TARGET_NAME} SHARED
  src/i2c_write.cpp
)
ament_target_dependencies(${TARGET_NAME} 
  rclcpp 
  rclcpp_components
)

rosidl_get_typesupport_target(cpp_typesupport_target ${PROJECT_NAME} "rosidl_typesupport_cpp")
target_link_libraries(${TARGET_NAME} ${cpp_typesupport_target})

ament_target_dependencies(${TARGET_NAME} rclcpp rclcpp_components)

rclcpp_components_register_nodes(${TARGET_NAME} "motor_control_node::MotorDataSubscriber")

install(TARGETS ${TARGET_NAME}
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
)

ament_package()
