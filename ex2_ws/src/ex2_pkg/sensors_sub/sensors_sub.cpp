#include "ros/ros.h" //per usare la libreria ros
#include "std_msgs/String.h"
#include "ex2_pkg_msgs/joints.h"

// Define the callback to be called when a message arrive
void receiverCallback(const ex2_pkg_msgs::joints::ConstPtr& joints){
    for(int i = 0; i<6; i++){
      ROS_INFO("Posizione del giunto %d: %d", i+1, joints->positions[i]);
    }
}

int main(int argc, char** argv){
    ros::init(argc, argv, "ex2_pkg_node2");
    ros::NodeHandle node;
  
    // Subscribe to the sensors topic
    ros::Subscriber sensors_sub = node.subscribe("sensors", 1000, receiverCallback);
    ros::spin();

    return 0;
}