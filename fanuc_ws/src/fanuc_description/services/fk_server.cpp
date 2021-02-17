#include <ros/ros.h>
#include <moveit_msgs/GetPositionFK.h>
#include <moveit/robot_state/robot_state.h>
#include <moveit/robot_state/conversions.h>
#include <moveit/robot_model/robot_model.h>
#include <moveit/robot_model_loader/robot_model_loader.h>
#include <eigen_conversions/eigen_msg.h>

bool compute_fk(moveit_msgs::GetPositionFK::Request &request,
                moveit_msgs::GetPositionFK::Response &response){
    
    response.fk_link_names = request.fk_link_names;

    // Look up the robot description on the ROS parameter server
    robot_model_loader::RobotModelLoader robot_model_loader("robot_description");
    
    // Get the constructed RobotModel
    robot_model::RobotModelPtr kinematic_model = robot_model_loader.getModel();

    // Create the state that maintains the configuration of the robot, from the previously obtained RobotModel
    robot_state::RobotState robot_state(kinematic_model);
    
    // Convert the robot state message to a moveit robot state
    moveit::core::robotStateMsgToRobotState(request.robot_state, robot_state);
    
    // robot_state.setToDefaultValues();
    // ROS_INFO("Link num: %d", request.fk_link_names.size());

    // Compute FK and prepare the response message for each link
    for (std::size_t i = 0; i < request.fk_link_names.size(); i++){
        const Eigen::Isometry3d& link_state = robot_state.getGlobalLinkTransform(request.fk_link_names[i]);
        
        /*
        ROS_INFO_STREAM("Translation: \n" << link_state.translation() << "\n");
        ROS_INFO_STREAM("Rotation: \n" << link_state.rotation() << "\n");
        Eigen::Quaterniond q(link_state.rotation());
        ROS_INFO_STREAM("Quaternion: " << q.x() << " " << q.y() << " " <<  q.z() << " " <<  q.w());
        */
       
        geometry_msgs::PoseStamped p;
        tf::poseEigenToMsg(link_state, p.pose);
        response.pose_stamped.push_back(p);
    }
    return true;
}

int main(int argc, char**argv){
    ros::init(argc, argv, "fk_server");
    ros::NodeHandle nh;
    ros::ServiceServer service = nh.advertiseService("FK", compute_fk);
    ros::spin();
    return 0;
}