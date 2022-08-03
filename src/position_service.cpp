/**
* \file position_service.cpp
* \brief Node random_position_server, which implements a random position service
* \author Akshita Kadambathil
* \version 0.1
* \date 03/08/2022
*/

#include "ros/ros.h"
#include "rt2_assignment1/RandomPosition.h"

/**
*
* \param double (M)
* \param double (N)
*
* \return double (randMToN)
*
* Description:
* Generate a random number between M and N
*/
double randMToN(double M, double N)
{     return M + (rand() / ( RAND_MAX / (N-M) ) ) ; }

/**
* 
* \param req
*   Service Request with x and y value range
* \param res
*   Service Response with x, y and theta values
*
* Description:
* Service callback bool (myrandom) that generates random values for my x, y and theta positions.
*/
bool myrandom (rt2_assignment1::RandomPosition::Request &req, rt2_assignment1::RandomPosition::Response &res){
    res.x = randMToN(req.x_min, req.x_max);
    res.y = randMToN(req.y_min, req.y_max);
    res.theta = randMToN(-3.14, 3.14);
    return true;
}

/**
*
* Description:
* Server that generates our random positions in our "/position_server" service
*/
int main(int argc, char **argv)
{
   ros::init(argc, argv, "random_position_server");
   ros::NodeHandle n;
   ros::ServiceServer service= n.advertiseService("/position_server", myrandom);
   ros::spin();

   return 0;
}
