#! /usr/bin/env python

##      @package rt2_assignment1
#       \file user_interface.py
#       \brief Python module for node implements an user interface
#       \author Akshita Kadambathil
#       \version 1.0
#       \date 03/08/2022
# 
#      \details
#  
#  Service:
#    /user_interface    
##  

import rospy
import time
from rt2_assignment1.srv import Command

def main():

##
#   This function initializes the ROS node and waits for the user to insert *start* or *stop* to control the robot, by relying on the 'rospy <http://wiki.ros.org/rospy/>'_ module.
#   
#   The user message is passed to the service 
#     ``user_interface``, 
#   advertised by :mod:`go_to_point`.
##

    rospy.init_node('user_interface')
    ui_client = rospy.ServiceProxy('/user_interface', Command)
    time.sleep(10)
    rate = rospy.Rate(20)
    x = int(input("\nPress 1 to start the robot "))
    while not rospy.is_shutdown():
        if (x == 1):
            ui_client("start")
            x = int(input("\nPress 0 to stop the robot "))
        else:
            print("Robot stoped, waiting for a new order")
            ui_client("stop")
            x = int(input("\nPress 1 to start the robot "))
            
if __name__ == '__main__':
    main()
