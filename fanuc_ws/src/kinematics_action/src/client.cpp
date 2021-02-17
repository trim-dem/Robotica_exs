#include <kinematics_action/inverse_kinematic_action.h>
#include <ros/ros.h>
#include <tf2_geometry_msgs/tf2_geometry_msgs.h>
#include <mutex>
#include <condition_variable>
#include <actionlib/client/simple_action_client.h>
#include <kinematics_action_msgs/GetIKSolutionsAction.h>

void handleGoalCompletionEvent(const actionlib::SimpleClientGoalState & state, const kinematics_action_msgs::GetIKSolutionsResultConstPtr & result);
void serializeIKSolution(std::ostringstream & ss, const moveit_msgs::RobotState & robot_state);
void handleGoalActiveEvent();
void handleFeedbackEvent(const kinematics_action_msgs::GetIKSolutionsFeedbackConstPtr & feedback);

std::mutex mutex;
std::condition_variable result_handled;


void handleGoalCompletionEvent(const actionlib::SimpleClientGoalState & state, const kinematics_action_msgs::GetIKSolutionsResultConstPtr & result){
    std::ostringstream ss;

    // check goal state
    if(state == actionlib::SimpleClientGoalState::StateEnum::SUCCEEDED)
    {
        int num_of_solutions = result -> ik_solutions.size();

        ss << "Goal achieved. " << state.getText() << std::endl;

        // For each solution print the joints position
        for(int i=0; i < num_of_solutions; i++)
        {
            serializeIKSolution(ss, result -> ik_solutions[i]);
            ss << std::endl;
        }

        ROS_INFO_STREAM(ss.str());

        ros::NodeHandle nh;

        // Instantiate the joint state publisher publishing on the joint_state topic
        ros::Publisher joint_state_publisher = nh.advertise<sensor_msgs::JointState>("joint_states",1);

        // load the robot model
        robot_model_loader::RobotModelLoader robot_model_loader("robot_description");
        robot_model::RobotModelPtr kinematic_model = robot_model_loader.getModel();

        // get the planning group name from the parameter server
        std::string planning_group_name;

        if(!nh.getParam("planning_group_name", planning_group_name))
        {
            ROS_ERROR("'planning group name' is undefined on the parameter server");
        }

        // get the planning group
        const robot_state::JointModelGroup *joint_model_group = kinematic_model -> getJointModelGroup(planning_group_name);
        
        // Instantiate the JointState message
        sensor_msgs::JointState joint_state_msg;
        
        // Fill the name field of the message
        joint_state_msg.name = joint_model_group -> getVariableNames();
        
        ROS_INFO("Publishing solutions...");
        // Set the waiting time between each solution publication
        ros::Duration sleep_time(2.0);
        
        for(int i=0; i < num_of_solutions; i++)
        {
            sleep_time.sleep();

            // Fill the position field of the message
            joint_state_msg.position = result -> ik_solutions[i].joint_state.position;
            joint_state_msg.header.stamp = ros::Time::now();

            // Publish the message
            joint_state_publisher.publish(joint_state_msg);
        }

        ROS_INFO("All solutions published!");

    }
    else
    {
        ss << "Goal aborted. " << state.getText();
        ROS_INFO_STREAM(ss.str());
    }

    result_handled.notify_all();
}

void handleGoalActiveEvent(){
    ROS_INFO("Inverse kinematics request sent to the IK resolution action server");
}

// Print the position for each joint of the robot
void serializeIKSolution(std::ostringstream & ss, const moveit_msgs::RobotState & robot_state){
    int n_joints = robot_state.joint_state.position.size();
    ss << "[";
    for(int i=0; i<n_joints; i++){
        ss << robot_state.joint_state.position[i];
        if(i != n_joints - 1) ss << ", ";
    }
    ss << "]";
}


// Print the solution obtained through feedback
void handleFeedbackEvent(const kinematics_action_msgs::GetIKSolutionsFeedbackConstPtr & feedback)
{
    std::ostringstream ss;

    ss << "Received IK solution: ";

    serializeIKSolution(ss, feedback -> ik_solution);

    ROS_INFO_STREAM(ss.str());
}

int main(int argc,char **argv)
{
    ros::init(argc, argv, "kinematics_action_client");

    // action client instantiation
    actionlib::SimpleActionClient<kinematics_action_msgs::GetIKSolutionsAction> client("ik_solver", true);

    // wait for server instantiation
    client.waitForServer();

    // goal instantation
    kinematics_action_msgs::GetIKSolutionsGoal goal;

    // fill the position field of the goal object
    goal.end_effector_pose.position.x = 1.0;
    goal.end_effector_pose.position.y = 0.0;
    goal.end_effector_pose.position.z = 1.0;

    // fill the orientation field of the goal object
    tf2::Quaternion quaternion;
    quaternion.setRPY(0.0, 0.0, 0.0);
    goal.end_effector_pose.orientation = tf2::toMsg(quaternion);

    // send the goal to the action server
    client.sendGoal(goal, &handleGoalCompletionEvent, &handleGoalActiveEvent, &handleFeedbackEvent);

    // wait for the result for a maximum of 30 seconds          
    if(!client.waitForResult(ros::Duration(30.0)))
        ROS_ERROR("The IK solver did not complete in the allotted time");

    // Wait for the release of the mutex from the goal handle callback
    std::unique_lock<std::mutex> lock(mutex);
    result_handled.wait(lock);

    ros::shutdown();
    return 0;
}