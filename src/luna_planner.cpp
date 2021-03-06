#include "ros/ros.h"
#include "std_msgs/Bool.h"
#include "lunabotics/State.h"
#include "lunabotics/Control.h"

void autonomyCallback(const std_msgs::Bool& msg)
{
	//Use msg to toggle autonomy
}

void stateCallback(const lunabotics::State& msg)
{
	//Use msg to recognize tilt angle. Raise alert if something is dangerous
}

int main(int argc, char **argv)
{
	ros::init(argc, argv, "luna_planner");
	ros::NodeHandle nodeHandle("lunabotics");
	ros::Subscriber autonomySubscriber = nodeHandle.subscribe("autonomy", 256, autonomyCallback);
	ros::Subscriber stateSubscriber = nodeHandle.subscribe("state", 256, stateCallback);
	ros::Publisher controlPublisher = nodeHandle.advertise<lunabotics::Control>("control", 256);
	ros::Rate loop_rate(50);
	
	ROS_INFO("State Machine ready"); 
	
	
	while (ros::ok()) {     		
		//Whenever needed send control message
		if (false) {
			lunabotics::Control controlMsg;
			controlMsg.motion.linear.x = 0;
			controlPublisher.publish(controlMsg);
		}
		
		ros::spinOnce();
		loop_rate.sleep();
	}
  
	return 0;
}
