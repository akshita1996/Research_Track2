cmake_minimum_required(VERSION 3.5)
project(rt2_assignment1)

# Default to C99
if(NOT CMAKE_C_STANDARD)
  set(CMAKE_C_STANDARD 99)
endif()

# Default to C++14
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
# uncomment the following section in order to fill in
# further dependencies manually.

find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(rclcpp_components REQUIRED)
find_package(rosidl_default_generators REQUIRED)

rosidl_generate_interfaces(${PROJECT_NAME}
    "srv/Command.srv"
    "srv/Position.srv"
    "srv/RandomPosition.srv"
)

add_library(position_srv SHARED
  src/position_service.cpp)
  
add_library(state_machine SHARED
  src/state_machine.cpp)

rclcpp_components_register_node(position_srv
  PLUGIN "rt2_assignment1::Position_srv"
  EXECUTABLE comp_position_srv)
  
rclcpp_components_register_node(state_machine
  PLUGIN "rt2_assignment1::State_Machine"
  EXECUTABLE comp_state_machine)
  
ament_target_dependencies(position_srv rclcpp rclcpp_components std_msgs)
ament_target_dependencies(state_machine rclcpp rclcpp_components std_msgs)

rosidl_target_interfaces(position_srv 
    ${PROJECT_NAME} "rosidl_typesupport_cpp"
)
rosidl_target_interfaces(state_machine 
    ${PROJECT_NAME} "rosidl_typesupport_cpp"
)

install(TARGETS
  position_srv state_machine
  comp_position_srv comp_state_machine  
  DESTINATION lib/${PROJECT_NAME})
  
install(
    DIRECTORY launch
    DESTINATION share/${PROJECT_NAME}
)


if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  # the following line skips the linter which checks for copyrights
  # uncomment the line when a copyright and license is not present in all source files
  #set(ament_cmake_copyright_FOUND TRUE)
  # the following line skips cpplint (only works in a git repo)
  # uncomment the line when this package is not in a git repo
  #set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()
