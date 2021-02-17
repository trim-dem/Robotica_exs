#include "ros/ros.h"
#include "std_msgs/String.h"
#include <sstream>
#include "ex2_pkg_msgs/joints.h"
#include "ex2_pkg_msgs/position.h"
#include <stdlib.h>
#include <time.h>

// The following code will simulate a set of sensors for encoder readings (position of 6 joints) 
int main(int argc, char **argv)
{
  ros::init(argc, argv, "ex2_pkg_node1");
  ros::NodeHandle node;

  // Initialize publisher
  ros::Publisher sensors_pub = node.advertise<ex2_pkg_msgs::joints>("sensors", 1000);
  ros::Rate loop_rate(1);
 
  while (ros::ok()){
    ex2_pkg_msgs::position pos;
    ex2_pkg_msgs::joints joints;

    // Create the joints message 
    joints.positions.resize(6);

    srand (time(NULL));
    
    // Fill the joints position files randomly
    for(int i = 0; i<6; i++){
      pos.position = (rand() % 10) + 1;
      joints.positions[i] = pos;
    }

    // Publish the message
    sensors_pub.publish(joints);
    ros::spinOnce();
    loop_rate.sleep();
  }


  return 0;
}
