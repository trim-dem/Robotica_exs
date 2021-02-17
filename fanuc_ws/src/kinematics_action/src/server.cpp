#include <kinematics_action/inverse_kinematic_action.h>
#include <ros/ros.h>

int main(int argc, char **argv){
    ros::init(argc, argv, "kinematics_action_server");

    kinematics_action::InverseKinematicsAction ik_action;

    ROS_INFO("Started inverse kinematics action server");

    ros::spin();

    ros::shutdown();
    return 0;
}