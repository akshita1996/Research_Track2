# First Assignment of the Research Track 2 course (Robotics Engineering / JEMARO, Unige) by akshita(5083574)
The package contains the nodes and the simulation environment for controlling a mobile robot in the Gazebo simulation environment.

## Table of contents
* [Requirements](#requirements)
* [General info](#general-info)
* [Description of the system](#description-of-the-system)
* [Codes and nodes](#codes-and-nodes) 
* [Running the code](#running-the-code) 
* [Futher information](#futher-information)

## Requirements

To properly oberserve and run correctly the behaviour of the mobile robot in your machine, it's necessary to have in your linux system:
- Ubuntu 20.04 
- ROS Noetic
- ROS2 Foxy
- Python3
- Gazebo
- Package ros1_bridge 

## General info

This package is responsible to control a mobile robot when receives a `go_to_point` command from the user.

## Description of the system

In this ROS2 package, it is possible to communicate with nodes declared in our ROS1 package in order to command a mobile robot in a Gazebo environment. 
The behaviour of the robot will depend on commands given by the user, they can be:

- **start:**
1. Receives a random value for X, Y and Theta
2. Based on the value of theta, rotates the robot until gets alligned with the goal value (X and Y)
3. Moves the robot towards the coodinates of X and Y
4. When reach the target, repeat the process

- **stop:** 
Because our user request is implemented as a service, the system will only read the user command when the robot reached its goal, being impossible to stop midway.

## Codes and nodes

In this package, at the src folder, it's possible to find 2 '.cpp' extension files, each one with it's own node:

- **position_service.cpp:** It generates and retreive a random position for x, y and theta when requested.
- **state_machine.cpp:** Send a *new* goal/objective to the *go_to_point* actoin server

## Running the code

To compile, follow the next steps:

### Communication with ROS1

1. Source your ROS version in a different shell
```
source ros.sh
roslaunch rt2_assignment1 bridge.launch
```

2. Source your ROS2 version in a different shell
```
source ros2.sh
ros2 run ros1_bridge dynamic_bridge
```

#### Launching the UDF mobile robot

3. Source your ROS12 version in a different shell
```
source bridge_rt2.sh
ros2 launch rt2_assignment1 sim_launch.py
```

## Futher information

To have acess to the documentation of this project, you need to go to:
- folder `doc`
- folder `html`
- double click on the file `files.html`

