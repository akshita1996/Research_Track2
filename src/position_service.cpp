/**
* \file position_service.cpp
* \brief Node random_position_server, which implements a random position service
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

/*Register a component that can be dynamically loaded at runtime*/
#include "rclcpp_components/register_node_macro.hpp"

using namespace std::chrono_literals;

using RandomPosition = rt2_assignment1::srv::RandomPosition;	//name of my .srv file, I'll get the informations from there

using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;

namespace rt2_assignment1{
/**
*
* Description:
* Generate a random number between M and N (/position_server)
*/

class Position_srv : public rclcpp::Node
{
public:
/**
*
* \param const rclcpp::NodeOptions (options)
*
* Description:
* Run the node (random_position_server) as a component
*/
	Position_srv(const rclcpp::NodeOptions & options)
	: Node("random_position_server", options)
	{
		service_ = this->create_service<RandomPosition>(
      "/position_server", std::bind(&Position_srv::myrandom, this, _1, _2, _3));
      }

private:
/**
*
* \param double (M)
* \param double (N)
*
* \return double (randMToN)
*
* Description:
* Generate a random number between M and N (/position_server)
*/
	double randMToN(double M, double N)
{     return M + (rand() / ( RAND_MAX / (N-M) ) ) ; }

/**
*
* \param rmw_request_id_t (request_header)
* \param RandomPosition::Request (req) - service with x and y range
* \param RandomPosition::Response (res) - service with x, y and theta
*
* Description:
* My service callback that generates the random values for the positions x, y and theta
*/

void myrandom (
	const std::shared_ptr<rmw_request_id_t> request_header,
	const std::shared_ptr<RandomPosition::Request> req, 
	const std::shared_ptr<RandomPosition::Response> res)
	{
	(void)request_header;
	res->x = randMToN(req->x_min, req->x_max);
	res->y = randMToN(req->y_min, req->y_max);
	res->theta = randMToN(-3.14, 3.14);
	}
rclcpp::Service<RandomPosition>::SharedPtr service_;  //my server for position
};
			}	

RCLCPP_COMPONENTS_REGISTER_NODE(rt2_assignment1::Position_srv)
	
