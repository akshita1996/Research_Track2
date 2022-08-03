/**
* \file state_machine.cpp
* \brief Implements a service to start or stop the robot, and calls the other two services to drive the robot
* \author Akshita Kadambathil
* \version 0.1
* \date 03/08/2022
*/

#include "ros/ros.h"
#include "rt2_assignment1/Command.h"
#include "rt2_assignment1/Position.h"
#include "rt2_assignment1/RandomPosition.h"

#include "rt2_assignment1/Assignment1Action.h"
#include "actionlib/client/simple_action_client.h"
#include "actionlib/client/terminal_state.h"

/*global variables*/
bool start = false;	// Command to move the robot
int options = 0;	// Actual robot state

/**
*
* \param req
*    Service Request with command (string)
* \param res
*    Service Response with the value of the bool (start)
* Description:
* Service callback to set the start and stop arguments state    
*/
bool user_interface(rt2_assignment1::Command::Request &req, rt2_assignment1::Command::Response &res)
{
    if (req.command == "start")
    	{
    	options = 1;
    	}
    else 
    	{
    	options = -1;
    	}
    return true;
}

/**
* 
* \param goal_state
*  Reached goal
* \param result
*  Result of the action
*
* Description:
* Callback that is activated when the goal of the action is reached
*/ 
void simple_done_callback(const actionlib::SimpleClientGoalState &get_state, const rt2_assignment1::Assignment1ResultConstPtr &result)
{    
  options = 2;	// Goal reached
}

int main(int argc, char **argv)
{
   ros::init(argc, argv, "state_machine");
   ros::NodeHandle n;
   
   ros::ServiceServer service= n.advertiseService("/user_interface", user_interface);
   ros::ServiceClient client_rp = n.serviceClient<rt2_assignment1::RandomPosition>("/position_server");
 /*Action client*/  actionlib::SimpleActionClient<rt2_assignment1::Assignment1Action> ac("go_to_point", true);
    
   /*local variables*/
   rt2_assignment1::Assignment1Goal goal; 
   
   /*Limit where the robot can go*/
   rt2_assignment1::RandomPosition rp;
   rp.request.x_max = 5.0;
   rp.request.x_min = -5.0;
   rp.request.y_max = 5.0;
   rp.request.y_min = -5.0;   
 
   while(ros::ok()){
   	ros::spinOnce();

   	switch (options){
   		case -1:	// User pressed "0", stop the robot
			ac.cancelGoal();	// Cancel the action and the goal
   			std::cout << "\nGoal was canceled" << std::endl;
   			options = 0;
   			break;
   		case 1:	// User pressed "1", go in the goal direction
   			client_rp.call(rp);
   			goal.x = rp.response.x;
   			goal.y = rp.response.y;
   			goal.theta = rp.response.theta;
   			std::cout << "\nGoing to the position: x= " << goal.x << " y= " <<goal.y << " theta = \n " << goal.theta << std::endl;
   			ac.sendGoal(goal, &simple_done_callback);	// Send the random positon as a goal to the action
   			options = 0;
   			break; 
   		case 2:	// End of action
   			actionlib::SimpleClientGoalState get_state = ac.getState();
   			/*Goal reached, end of action*/
   			if(get_state == actionlib::SimpleClientGoalState::SUCCEEDED)
			{
			std::cout << "\nOn Goal" << std::endl;
			options = 1;	
			}
			/*Goal not reached, robot has stopped, end of action*/
			else if(get_state == actionlib::SimpleClientGoalState::PREEMPTED)
			{
			std::cout << "\nCanceled" << std::endl;
			options = 0;	
			}
			/*Failled to reach the goal, end of action*/
			else
			{
			std::cout << "\nFailed" << std::endl;
			options = 0;	
			}
   	}
   }
   return 0;
}
