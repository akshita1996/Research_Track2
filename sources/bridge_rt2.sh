#!/bin/bash

#ros=/opt/ros/noetic/setup.bash
#ros_local=/home/${USER}/my_ros_ws/devel/setup.bash

#ros2=/opt/ros/foxy/setup.bash
#ros2_local=/home/${USER}/my_ros2/install/local_setup.bash

gnome-terminal --tab --title="ros" -- bash -c "source ros.sh; roslaunch rt2_assignment1 bridge.launch"

gnome-terminal --tab --title="ros1_bridge" -- bash -c "sleep 4; source ros.sh; source ros2.sh; ros2 run ros1_bridge dynamic_bridge"

gnome-terminal --tab --title="ros2" -- bash -c "sleep 7; source ros2.sh; ros2 launch rt2_assignment1 sim_launch.py"
