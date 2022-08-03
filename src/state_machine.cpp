/**
* \file state_machine.cpp
* \brief Implements a service to start or stop the robot, and calls the other two services to drive the robot
* \author Akshita Kadambathil
* \version 0.1
* \date 03/08/2022
*/

#include <chrono>
#include <memory>
#include <inttypes.h>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"

#include "rt2_assignment1/srv/random_position.hpp"
#include "rt2_assignment1/srv/command.hpp"
#include "rt2_assignment1/srv/position.hpp"

#include "rclcpp_components/register_node_macro.hpp"

using namespace std::chrono_literals;

using Command = rt2_assignment1::srv::Command;	
using Position = rt2_assignment1::srv::Position;
using RandomPosition = rt2_assignment1::srv::RandomPosition;	

using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;	

namespace rt2_assignment1{

/**
*
* Description:
* Receives the user's request to start moving the mobile robot, the random pose is received by the /position_server and then sent to go_to_point service, the process will repeat in a loop unitl the user send a command for the robot stop
*/
class State_Machine : public rclcpp::Node
{
public:

/**
* 
* \param rclcpp::NodeOptions (options)
*  run the node as component
*
* Description:
* Initialize the variables, server and 2 clients
*/ 
	State_Machine(const rclcpp::NodeOptions & options)
	: Node("state_machine", options)
	{
	start = false;
	position_reached = true;
	
	/*Initialization client and service*/  
	
      service_ = this->create_service<Command>(
      "/user_interface", std::bind(&State_Machine::user_interface, this, _1, _2, _3));  
      client_rp = this->create_client<RandomPosition>("/position_server");
      
      while (!client_rp->wait_for_service(std::chrono::seconds(1))){
      	if (!rclcpp::ok()) {
      RCLCPP_ERROR(this->get_logger(), "client interrupted while waiting for service to appear.");
      return;
    }
    RCLCPP_INFO(this->get_logger(), "waiting for service to appear...");
  }
  
  client_p = this->create_client<Position>("/go_to_point");
  
  while (!client_p->wait_for_service(std::chrono::seconds(1))){
     if (!rclcpp::ok()) {
      RCLCPP_ERROR(this->get_logger(), "client interrupted while waiting for service to appear.");
      return;
    }
    RCLCPP_INFO(this->get_logger(), "waiting for service to appear...");
  }
  
  rp_req = std::make_shared<RandomPosition::Request>();
  rp_res = std::make_shared<RandomPosition::Response>();
  p_req = std::make_shared<Position::Request>();
  
  rp_req->x_max = 5.0;
  rp_req->x_min = -5.0;
  rp_req->y_max = 5.0;
  rp_req->y_min = -5.0;
}        

/**
* 
* \param rmw_request_id_t (request_header)
*  Call header
*
* \param Command::Request (req)
*  Service Request with command (string)
*
* \param Command::Response (res)
*  Service Response with the value of the bool (start)
*
* Description:
* Service callback to set the start and stop arguments state
*/ 
  void user_interface(
  	 const std::shared_ptr<rmw_request_id_t> request_header,
  	 const std::shared_ptr<Command::Request> req,
  	 const std::shared_ptr<Command::Response> res)
  	{
  	(void) request_header;
  		if (req->command == "start")
  		{
  		start = true;
  		Go_To_Point();
  		}
  		else
  		{
  		start = false;
  		}
  	res->ok = start;
  	RCLCPP_INFO(this->get_logger(), "Received request %s", req->command.c_str());
  	}  	

/**
*
* Description:
* Receive and set a new goal position, this position comes from /position_server and go to /go_to_point as a request to set the new goal position for the robot.
*/
void Go_To_Point(){
    
    myrandom_call();
    
    position_reached = false; // receive a new goal
    
    p_req->x = rp_res->x;
    p_req->y = rp_res->y;
    p_req->theta = rp_res->theta;
    
    RCLCPP_INFO(this->get_logger(), "Going to the position: x= %f y= %f theta= %f",
            p_req->x, p_req->y, p_req->theta);
    auto point_reached_callback =
              [this](rclcpp::Client<rt2_assignment1::srv::Position>::SharedFuture future){(void)future; position_reached = true;
               RCLCPP_INFO(this->get_logger(), "Goal reached!");};
    auto future_result = client_p->async_send_request(p_req, point_reached_callback);
  }

/**
*
* Description:
* Function that call my rnaodm position service. The positon of the goal received from the /position_server service is stored in the variable rp_res as a "copy"
*/
void myrandom_call(){  
	auto rp_res_callback = [this](rclcpp::Client<rt2_assignment1::srv::RandomPosition>::SharedFuture future){rp_res = future.get();};
	auto future_result = client_rp->async_send_request(rp_req, rp_res_callback);
}

/*int main() part*/

/*global variables*/
bool start, position_reached;

rclcpp::Service<Command>::SharedPtr service_;
rclcpp::Client<RandomPosition>::SharedPtr client_rp;
rclcpp::Client<Position>::SharedPtr client_p;

std::shared_ptr<RandomPosition::Request> rp_req;
std::shared_ptr<RandomPosition::Response> rp_res;
std::shared_ptr<Position::Request> p_req;

};
}

RCLCPP_COMPONENTS_REGISTER_NODE(rt2_assignment1::State_Machine)

