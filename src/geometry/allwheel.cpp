#include "allwheel.h"
#include "ros/ros.h"
#include "basic.h"
using namespace lunabotics;


AllWheelGeometry::AllWheelGeometry(Point left_front, Point left_rear, Point right_front, Point right_rear): lf(left_front), lr(left_rear), rf(right_front), rr(right_rear), geometryAcquired(false) {
}

AllWheelGeometry::AllWheelGeometry(AllWheelGeometry *copy) 
{
	this->lf = copy->left_front();
	this->lr = copy->left_rear();
	this->rf = copy->right_front();
	this->rr = copy->right_rear();
	this->_wheel_offset = copy->wheel_offset();
	this->_wheel_radius = copy->wheel_radius();
	this->geometryAcquired = copy->geometryAcquired;
}

AllWheelGeometry::~AllWheelGeometry()
{
}

bool AllWheelGeometry::calculateAngles(Point ICR, float &left_front, float &right_front, float &left_rear, float &right_rear)
{
//	ROS_INFO("Joint positions (%.2f,%.2f) (%.2f,%.2f) (%.2f,%.2f) (%.2f,%.2f)", this->lf.x, this->lf.y, this->rf.x, this->rf.y, this->lr.x, this->lr.y, this->rr.x, this->rr.y);
	
	bool ICROnRight = ICR.y < this->rf.y;
	bool ICROnLeft = ICR.y > this->lf.y;
	bool ICROnTop = ICR.x > this->lf.x;
	bool ICROnBottom = ICR.x < this->lr.x;
	
	if ((ICROnTop || ICROnBottom) && !(ICROnRight || ICROnLeft)) {
		ROS_ERROR("Unhandled ICR position");
		return false;
	}
	else {
		if (ICROnRight) {
			//ICR is on the right from the robot
		//	ROS_INFO("ICR on the right");
			
			double offset = -ICR.y+this->rf.y;
		//	ROS_INFO("Right offset %f top %f bottom %f", offset, this->rf.x-ICR.y, -this->rr.x+ICR.y);
			right_front = -atan2(this->rf.x-ICR.x, offset);
			right_rear = atan2(-this->rr.x+ICR.x, offset);
			offset += (this->lf.y-this->rf.y);
		//	ROS_INFO("Left offset %f top %f bottom %f", offset, this->lf.x-ICR.y, -this->lr.x+ICR.y);
			left_front = -atan2(this->lf.x-ICR.x, offset);
			left_rear = atan2(-this->lr.x+ICR.x, offset);
		}
		else if (ICROnLeft) {
			//ICR is on the left from the robot
		//	ROS_INFO("ICR on the left");
			
			double offset = ICR.y-this->lf.y;
			left_front = atan2(this->lf.x-ICR.x, offset);
			left_rear = -atan2(-this->lr.x+ICR.x, offset);
		//	ROS_INFO("Left offset %f top %f bottom %f", offset, this->lf.x-ICR.y, -this->lr.x+ICR.y);
			offset += (this->lf.y-this->rf.y);
			right_front = atan2(this->rf.x-ICR.x, offset);
			right_rear = -atan2(-this->rr.x+ICR.x, offset);
	//		ROS_INFO("Right offset %f top %f bottom %f", offset, this->rf.x-ICR.y, -this->rr.x+ICR.y);
		}
		else {
			//ICR is underneath the robot
		//	ROS_INFO("ICR in between");
			
			double offset = this->lf.y-ICR.y;
			left_front = -atan2(this->lf.x-ICR.x, offset);
			left_rear = atan2(-this->lr.x+ICR.x, offset);
	//		ROS_INFO("Left offset %f top %f bottom %f", offset, this->lf.x-ICR.y, -this->lr.x+ICR.y);
			offset = -this->rf.y+ICR.y;
			right_front = atan2(this->rf.x-ICR.x, offset);
			right_rear = -atan2(-this->rr.x+ICR.x, offset);
	//		ROS_INFO("Right offset %f top %f bottom %f", offset, this->rf.x-ICR.y, -this->rr.x+ICR.y);
		}
	//	ROS_INFO("Calculated angles are %.2f | %.2f | %.2f | %.2f", left_front, right_front, left_rear, right_rear);
	
	}
	return true;
}

bool AllWheelGeometry::calculateVelocities(Point ICR, float center_velocity, float &left_front, float &right_front, float &left_rear, float &right_rear)
{
	//Coordinate frames are different
	
//	ROS_INFO("Joint positions (%.2f,%.2f) (%.2f,%.2f) (%.2f,%.2f) (%.2f,%.2f)", this->lf.x, this->lf.y, this->rf.x, this->rf.y, this->lr.x, this->lr.y, this->rr.x, this->rr.y);
	bool ICROnRight = ICR.y < this->rf.y;
	bool ICROnLeft = ICR.y > this->lf.y;
	bool ICROnTop = ICR.x > this->lf.x;
	bool ICROnBottom = ICR.x < this->lr.x;
	
	if ((ICROnTop || ICROnBottom) && !(ICROnRight || ICROnLeft)) {
		ROS_ERROR("Unhandled ICR position");
		return false;
	}
	else {		
		double left_front_shoulder = distance(ICR, this->lf);
		double right_front_shoulder = distance(ICR, this->rf);
		double left_rear_shoulder = distance(ICR, this->lr);
		double right_rear_shoulder = distance(ICR, this->rr);
		
	//	ROS_INFO("Shoulders are are %.2f | %.2f | %.2f | %.2f", left_front_shoulder, right_front_shoulder, left_rear_shoulder, right_rear_shoulder);
		
		if (ICROnRight) {
			//ROS_INFO("ICR on the right");
			right_front_shoulder -= this->_wheel_offset;
			right_rear_shoulder -= this->_wheel_offset;
			left_front_shoulder += this->_wheel_offset;
			left_rear_shoulder += this->_wheel_offset;
		}
		else if (ICROnLeft) {
			//ICR is on the left from the robot
			//ROS_INFO("ICR on the left");
			right_front_shoulder += this->_wheel_offset;
			right_rear_shoulder += this->_wheel_offset;
			left_front_shoulder -= this->_wheel_offset;
			left_rear_shoulder -= this->_wheel_offset;
		}
		else {
			//ICR is underneath the robot
			//ROS_INFO("ICR in between");
			right_front_shoulder += this->_wheel_offset;
			right_rear_shoulder += this->_wheel_offset;
			left_front_shoulder += this->_wheel_offset;
			left_rear_shoulder += this->_wheel_offset;
		}
		
		//ROS_INFO("Shoulders are are %.2f | %.2f | %.2f | %.2f", left_front_shoulder, right_front_shoulder, left_rear_shoulder, right_rear_shoulder);
		
		Point zeroPoint; zeroPoint.x = 0; zeroPoint.y = 0;
		double center_shoulder = distance(ICR, zeroPoint);
		double ang_vel = center_velocity/center_shoulder;
		
		double left_front_vel = left_front_shoulder*ang_vel;
		double right_front_vel = right_front_shoulder*ang_vel;
		double left_rear_vel = left_rear_shoulder*ang_vel;
		double right_rear_vel = right_rear_shoulder*ang_vel;
		
		left_front = left_front_vel/this->_wheel_radius;
		right_front = right_front_vel/this->_wheel_radius;
		left_rear = left_rear_vel/this->_wheel_radius;
		right_rear = right_rear_vel/this->_wheel_radius;
		
		if (!ICROnLeft && !ICROnRight) {
			if (ICR.y < 0) {
				right_front *= -1;
				right_rear *= -1;
			}
			else {
				left_front *= -1;
				left_rear *= -1;
			}
		}
		
		if (isinf(left_front) || isnan(left_front) || isinf(right_front) || isnan(right_front) || isinf(left_rear) || isnan(left_rear) || isinf(right_rear) || isnan(right_rear)) {
	//		ROS_INFO("INF or NAN");
			left_front = right_front = left_rear = right_rear = 0;	
		}
				
	//	ROS_INFO("Calculated velocities are %.2f | %.2f | %.2f | %.2f", left_front, right_front, left_rear, right_rear);
	}
	return true;
}

void AllWheelGeometry::set_left_front(Point new_point)
{
	this->lf = new_point;
}

void AllWheelGeometry::set_left_rear(Point new_point)
{
	this->lr = new_point;
}

void AllWheelGeometry::set_right_front(Point new_point)
{
	this->rf = new_point;
}

void AllWheelGeometry::set_right_rear(Point new_point)
{
	this->rr = new_point;
}

void AllWheelGeometry::set_wheel_offset(float new_offset)
{
	this->_wheel_offset = new_offset;
}

void AllWheelGeometry::set_wheel_radius(float new_radius)
{
	this->_wheel_radius = new_radius;
}

void AllWheelGeometry::set_wheel_width(float new_width)
{
	this->_wheel_width = new_width;
}

Point AllWheelGeometry::left_front()
{
	return this->lf;
}
Point AllWheelGeometry::left_rear()
{
	return this->lr;
}
Point AllWheelGeometry::right_front()
{
	return this->rf;
}
Point AllWheelGeometry::right_rear()
{
	return this->rr;
}

float AllWheelGeometry::wheel_offset()
{
	return this->_wheel_offset;
}

float AllWheelGeometry::wheel_radius()
{
	return this->_wheel_radius;
}

float AllWheelGeometry::wheel_width()
{
	return this->_wheel_width;
}

bool lunabotics::validateAngles(float &left_front, float &right_front, float &left_rear, float &right_rear)
{
	bool result = true;
	if (left_front > GEOMETRY_INNER_ANGLE_MAX) {
		result = false;
		left_front = GEOMETRY_INNER_ANGLE_MAX;
	}
	else if (left_front < -GEOMETRY_OUTER_ANGLE_MAX) {
		result = false;
		left_front = -GEOMETRY_OUTER_ANGLE_MAX;
	}
	
	if (right_front > GEOMETRY_OUTER_ANGLE_MAX) {
		result = false;
		right_front = GEOMETRY_OUTER_ANGLE_MAX;
	}
	else if (right_front < -GEOMETRY_INNER_ANGLE_MAX) {
		result = false;
		right_front = -GEOMETRY_INNER_ANGLE_MAX;
	}
	
	if (left_rear > GEOMETRY_OUTER_ANGLE_MAX) {
		result = false;
		left_rear = GEOMETRY_OUTER_ANGLE_MAX;
	}
	else if (left_rear < -GEOMETRY_INNER_ANGLE_MAX) {
		result = false;
		left_rear = -GEOMETRY_INNER_ANGLE_MAX;
	}
	
	if (right_rear > GEOMETRY_INNER_ANGLE_MAX) {
		result = false;
		right_rear = GEOMETRY_INNER_ANGLE_MAX;
	}
	else if (right_rear < -GEOMETRY_OUTER_ANGLE_MAX) {
		result = false;
		right_rear = -GEOMETRY_OUTER_ANGLE_MAX;
	}
	return result;
}

Point AllWheelGeometry::point_outside_base_link(Point ICR)
{
	if (ICR.y < 0 && ICR.y > this->rf.y) {
		ICR.y = this->rf.y-this->_wheel_offset/2;
	}
	else if (ICR.y >= 0 && ICR.y < this->lf.y) {
		ICR.y = this->lf.y+this->_wheel_offset/2;
	}
	return ICR;
}
