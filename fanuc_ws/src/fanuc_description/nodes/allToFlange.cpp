#include "ros/ros.h"
#include <tf2_ros/transform_listener.h>
#include <tf2_geometry_msgs/tf2_geometry_msgs.h>

int main(int argc, char ** argv)
{
  //Initialize ROS
  ros::init(argc, argv, "tf_echo", ros::init_options::AnonymousName);
  ros::NodeHandle nh("~");

  double rate_hz;
  rate_hz = 1;
  ros::Rate rate(rate_hz);

  std::string source_frameid = "flange";

  tf2_ros::Buffer tfBuffer;
  tf2_ros::TransformListener tfListener(tfBuffer);

  const char *frames[] = {"base_link", "link1", "link2", "link3", "link4", "link5", "link6"};

  while(nh.ok()){

    // Get the transform between the flange link and all the other frames
    for(int i = 0; i < sizeof(frames)/sizeof(*frames); i++){
      geometry_msgs::TransformStamped transformStamped;
      try{
        transformStamped = tfBuffer.lookupTransform(frames[i], source_frameid, ros::Time(0));
      }
      catch (tf2::TransformException &ex) {
        ROS_WARN("%s",ex.what());
        continue;
      }

      // Convert the obtained transform message to its equivalent tf2 representation
      tf2::Stamped<tf2::Transform> tr;
      tf2::fromMsg(transformStamped, tr);
      
      // Get the rotation matrix
      double yaw, pitch, roll;
      tf2::Matrix3x3 rot_mat = tr.getBasis();
      
      // Get the matrix represented as roll pitch and yaw about fixed axes XYZ
      rot_mat.getRPY(roll, pitch, yaw);

      // Get the quaternion representing the rotation
      tf2::Quaternion q = tr.getRotation();
      
      // Get the origin vector translation
      tf2::Vector3 v = tr.getOrigin();

      // Get the angle [0, 2Pi] of rotation represented by the quaternion
      double teta = q.getAngle();

      // Get the axis of the rotation represented by the quaternion
      tf2::Vector3 axis = q.getAxis();

      // Print all the obtained information
      std::cout << "Source: " << frames[i] << " - Destination: " << source_frameid << std::endl; 
      std::cout << "- Translation: [" << v.getX() << ", " << v.getY() << ", " << v.getZ() << "]" << std::endl;
      std::cout << "- Rotation: in Quaternion [" << q.getX() << ", " << q.getY() << ", " 
                << q.getZ() << ", " << q.getW() << "]" << std::endl
                << "            in RPY (radian) [" <<  roll << ", " << pitch << ", " << yaw << "]" << std::endl;
      std::cout << "- Rotation Matrix: " << std::endl;
      for(int j = 0; j < 3; j++){
        for(int k = 0; k < 3; k++){
          std::cout << rot_mat[j][k] << "\t";
        }
        std::cout << std::endl;
      }

      std::cout << "- Axis/Angle: " << std::endl;
      std::cout << "Axis = [" << axis.getX() << ", " << axis.getY() << ", " << axis.getZ() << "]" << std::endl; 
      std::cout << "Angle = " << teta << std::endl;
      std::cout << std::endl << "++++++++++++++++++" << std::endl;
      
    }
  std::cout << std::endl << std::endl << "--------------------" << std::endl << std::endl;
  rate.sleep();
  }
return 0;
}