#include <ros/ros.h>
#include <moveit_msgs/GetPositionFK.h>
#include <moveit/robot_state/robot_state.h>
#include <moveit/robot_state/conversions.h>
#include <moveit/robot_model/robot_model.h>
#include <moveit/robot_model_loader/robot_model_loader.h>

void service_result_handle(moveit_msgs::GetPositionFK service){
    ROS_INFO_STREAM("SUCCESS: " << service.response.pose_stamped.size());
        for (std::size_t i = 0; i < service.response.pose_stamped.size(); i++){
            ROS_INFO_STREAM("Link name: "  << service.response.fk_link_names[i]);          
            ROS_INFO_STREAM("position: ["  << service.response.pose_stamped[i].pose.position.x << " "
                                    << service.response.pose_stamped[i].pose.position.y << " "
                                    << service.response.pose_stamped[i].pose.position.z << "]");
                                    
            ROS_INFO_STREAM("quaternion: ["<< service.response.pose_stamped[i].pose.orientation.x << " "
                                    << service.response.pose_stamped[i].pose.orientation.y << " "
                                    << service.response.pose_stamped[i].pose.orientation.z << " "
                                    << service.response.pose_stamped[i].pose.orientation.w << "]\n");
    }
}

int main(int argc, char **argv){
    ros::init(argc, argv, "fk_client");
    ros::NodeHandle nh;
    
    // Creation of the client of my servive server
    ros::ServiceClient client = nh.serviceClient<moveit_msgs::GetPositionFK>("FK");
    
    // Creation of the client of the move_group service server
    ros::ServiceClient mg_client = nh.serviceClient<moveit_msgs::GetPositionFK>("compute_fk");
    
    moveit_msgs::GetPositionFK service;

    
    // Look up the robot description on the ROS parameter server
    robot_model_loader::RobotModelLoader robot_model_loader("robot_description");

    // Get the constructed RobotModel
    robot_model::RobotModelPtr kinematic_model = robot_model_loader.getModel();

    // Print the frame in which the transforms for this model are computed
    ROS_INFO("Model frame: %s", kinematic_model->getModelFrame().c_str());

    ros::Rate loop_rate(1);
    while (ros::ok()){
        // Create the state that maintains the configuration of the robot, from the previously obtained RobotModel
        robot_state::RobotState robotState(kinematic_model);
        robotState.updateLinkTransforms();

        // Convert RobotState to RobotState message
        moveit::core::robotStateToRobotStateMsg(robotState, service.request.robot_state);
        
        // Fill the request with the link name
        std::vector<moveit::core::LinkModel*> link_models = kinematic_model->getLinkModels(); // Get the array of links
        /*for (std::size_t i = 0; i < link_models.size(); i++){
            ROS_INFO("Link name: %s", link_models[i]->getName().c_str());
            service.request.fk_link_names.push_back(link_models[i]->getName());
        }*/

        service.request.fk_link_names.clear();
        service.request.fk_link_names.push_back(link_models[link_models.size()-1]->getName());

        // Call my service and print the obtained position and orientation
        if(client.call(service)) {
            ROS_INFO_STREAM("MY SERVICE SUCCESS");
            service_result_handle(service);
        }
        else{
            ROS_ERROR("MY SERVICE FAILURE");
        }
        // Call move group service and print the obtained position and orientation in order to compare with previous results and check correctness
        if(mg_client.call(service)) {
            ROS_INFO_STREAM("MOVE GROUP SERVICE SUCCESS");
            service_result_handle(service);
        }
        else{
            ROS_ERROR("MOVE_GROUP SERVICE FAILURE");
        }
        ros::spinOnce();
        loop_rate.sleep();
    }
    return 0;
}