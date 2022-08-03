#! /usr/bin/env python

##      @package rt2_assignment1
#       \file go_to_point.py
#       \brief Python module for piloting the robot to the target
#       \author Akshita Kadambathil
#       \version 1.0
#       \date 03/08/2022
# 
#      \details
#
#      Servers: 
#         Action: <BR>
#             /go_to_point (rt2_assignment1.action.Assignment1Action)
#         Service: <BR>
#             /vel (rt2_assignment1.srv.Velocity)
#      Publishes to: <BR>
#         /cmd_vel (geometry_msgs.msg.Twist)
##     

import rospy
from geometry_msgs.msg import Twist, Point
from nav_msgs.msg import Odometry
from tf import transformations
from rt2_assignment1.srv import Position
import math

import rt2_assignment1.msg 
import actionlib
import actionlib.msg
import motion_plan.msg

#FOR ASSIGNMENT 2#
import numpy as np
from rt2_assignment1.srv import Command, Velocity, VelocityResponse

# robot state variables
position_ = Point()
##
# Point: actual robot position
##

yaw_ = 0
position_ = 0
state_ = 0
pub_ = None

# action server
act_s = None

#FOR ASSIGNMENT2
# velocity server
vel_s = None

# parameters for control
yaw_precision_ = math.pi / 9  # +/- 20 degree allowed
yaw_precision_2_ = math.pi / 90  # +/- 2 degree allowed
dist_precision_ = 0.1
kp_a = -3.0 
kp_d = 0.2
ub_a = 0.6  #angular speed
lb_a = -0.5
ub_d = 0.6  #linear speed

#FOR ASSIGNMENT2
def Vel_srv(req):

    global ub_d    
    global ub_a 
        
    ub_d = req.linear
    ub_a = req.angular
    
    rospy.logdebug("Linear: [%f] and Angular:[%f]", req.linear, req.angular)
    
    return VelocityResponse()    
    
def clbk_odom(msg):
##  
#    Odometry callback
#
#    Arguments: Odometry message (msg)
#    
#    Description:
#    Receives from Odometry message the values of X, Y and theta
##    

    global position_
    global yaw_

    # position
    position_ = msg.pose.pose.position

    # yaw
    quaternion = (
        msg.pose.pose.orientation.x,
        msg.pose.pose.orientation.y,
        msg.pose.pose.orientation.z,
        msg.pose.pose.orientation.w)
    euler = transformations.euler_from_quaternion(quaternion)
    yaw_ = euler[2]


def change_state(state):
##   
#    Arguments: state (int)
#    
#    Description:
#    Update the global state
##    

    global state_
    state_ = state
    print ('State changed to [%s]' % state_)


def normalize_angle(angle):
##   
#    Arguments: float (angle) -> input
#                       float (angle) -> return		
#   
#    Description:
#    Normalize the angle between pi and -pi
##   

    if(math.fabs(angle) > math.pi):
        angle = angle - (2 * math.pi * angle) / (math.fabs(angle))
    return angle

def fix_yaw(des_pos):
##   
#    Arguments: float (des_yaw)
#                   int (next_state)
#              
#    Description: 
#    Orient the robot in direction of goal position (x,y) or achieve it.
#    It can also change to a new state depending of the actual orientation.	
##    

    desired_yaw = math.atan2(des_pos.y - position_.y, des_pos.x - position_.x)
    err_yaw = normalize_angle(desired_yaw - yaw_)
    rospy.loginfo(err_yaw)
    twist_msg = Twist()
    if math.fabs(err_yaw) > yaw_precision_2_:
        twist_msg.angular.z = kp_a*err_yaw
        if twist_msg.angular.z > ub_a:
            twist_msg.angular.z = ub_a
        elif twist_msg.angular.z < lb_a:
            twist_msg.angular.z = lb_a
    pub_.publish(twist_msg)
    # state change conditions
    if math.fabs(err_yaw) <= yaw_precision_2_:
        #print ('Yaw error: [%s]' % err_yaw)
        change_state(1)


def go_straight_ahead(des_pos):
##   
#    Arguments: point (des_pos)
#                       position (desired_) for x, y
#    
#    Description:           
#    Set both angular and linear speed depending on the distance between the robot the goal
##    

    desired_yaw = math.atan2(des_pos.y - position_.y, des_pos.x - position_.x)
    err_yaw = desired_yaw - yaw_
    err_pos = math.sqrt(pow(des_pos.y - position_.y, 2) +
                        pow(des_pos.x - position_.x, 2))
    err_yaw = normalize_angle(desired_yaw - yaw_)
    rospy.loginfo(err_yaw)

    if err_pos > dist_precision_:
        twist_msg = Twist()
        twist_msg.linear.x = 0.3	##
        if twist_msg.linear.x > ub_d:
            twist_msg.linear.x = ub_d

        twist_msg.angular.z = kp_a*err_yaw
        pub_.publish(twist_msg)
    else: # state change conditions
        #print ('Position error: [%s]' % err_pos)
        change_state(2)

    # state change conditions
    if math.fabs(err_yaw) > yaw_precision_:
        #print ('Yaw error: [%s]' % err_yaw)
        change_state(0)

def fix_final_yaw(des_yaw):
    err_yaw = normalize_angle(des_yaw - yaw_)
    rospy.loginfo(err_yaw)
    twist_msg = Twist()
    if math.fabs(err_yaw) > yaw_precision_2_:
        twist_msg.angular.z = kp_a*err_yaw
        if twist_msg.angular.z > ub_a:
            twist_msg.angular.z = ub_a
        elif twist_msg.angular.z < lb_a:
            twist_msg.angular.z = lb_a
    pub_.publish(twist_msg)
    # state change conditions
    if math.fabs(err_yaw) <= yaw_precision_2_:
        #print ('Yaw error: [%s]' % err_yaw)
        change_state(3)
        
def done():
##   
#    Description:
#    Stop the robot setting both angular and linear velocity to 0
##   

    twist_msg = Twist()
    twist_msg.linear.x = 0
    twist_msg.angular.z = 0
    pub_.publish(twist_msg)
    
def go_to_point(goal):		##Action goal
##   
#    Arguments: goal position (goal) for x, y and theta
#    
#    Description:
#    Set an action deppending on the robot actual state. 
#    Direct communication with the state machine code, it will run in a loop until the user cancel/preempt the action
##   

    global act_s
		
    desired_position = Point()
    desired_position.x = goal.x
    #desired_position.x = req.x
    desired_position.y = goal.y
    #desired_position.y = req.y
    des_yaw = goal.theta
    #des_yaw = req.theta
    
    rate = rospy.Rate(20)
    objective = True
    change_state(0)   
    
    feedback = rt2_assignment1.msg.Assignment1Feedback()
    result = rt2_assignment1.msg.Assignment1Result()
    
    while not rospy.is_shutdown():
    
    	#if action_server.set_aborted():
    	if act_s.is_preempt_requested():
    		rospy.loginfo('Goal was preempted')
    		act_s.set_preempted()
    		objective = False
    		done()
    		break
    	else:
	    	if state_ == 0:
	    		fix_yaw(desired_position)
	    		feedback.feedback = 'Rotating to goal'
	    	elif state_ == 1:
	    		go_straight_ahead(desired_position)
	    		feedback.feedback = 'Moving towards target'
	    	elif state_ == 2:
	    		feedback.feedback = 'Goal X and Y'
	    		fix_final_yaw(des_yaw)
	    	elif state_ == 3:
	    		feedback.feedback = 'You reached the GOAL !!! Congratulations'
	    		done()
	    		break
	    	else:
                	rospy.logerr('Unknown state!')
                
	    	act_s.publish_feedback(feedback)
	    	rate.sleep()
    
    if objective:
    	result.result = objective
    	rospy.loginfo('Objective complete')
    	act_s.set_succeeded(result)
    	#break
    	
def main():
    global pub_, act_s, vel_s
    rospy.init_node('go_to_point')
    pub_ = rospy.Publisher('/cmd_vel', Twist, queue_size=1)
    sub_odom = rospy.Subscriber('/odom', Odometry, clbk_odom)
    act_s = actionlib.SimpleActionServer('/go_to_point', rt2_assignment1.msg.Assignment1Action, go_to_point)
    
    #FOR ASSIGNMENT2    
    vel_s = rospy.Service('/vel', Velocity, Vel_srv)
    
    act_s.start()    
    
    rospy.spin()    

if __name__ == '__main__':
    main()
