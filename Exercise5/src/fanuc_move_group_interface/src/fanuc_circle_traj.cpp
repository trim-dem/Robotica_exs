#include <ros/ros.h>
#include <moveit/move_group_interface/move_group_interface.h>
#include <moveit/planning_scene_interface/planning_scene_interface.h>

#include <moveit_msgs/DisplayRobotState.h>
#include <moveit_msgs/DisplayTrajectory.h> 
#include <moveit_msgs/RobotTrajectory.h>

#include <moveit/robot_trajectory/robot_trajectory.h>

#include <moveit_msgs/AttachedCollisionObject.h>
#include <moveit_msgs/CollisionObject.h>

#include <moveit_visual_tools/moveit_visual_tools.h>
#include <moveit/planning_pipeline/planning_pipeline.h>
#include <moveit/robot_state/robot_state.h>
#include <moveit/robot_state/conversions.h>

#include <angles/angles.h>

void computeVectorsDifference(
    Eigen::VectorXd & diff,
    const Eigen::VectorXd & minuend,
    const Eigen::VectorXd & subtrahend,
    const moveit::core::JointModelGroup * jmg);

int main(int argc, char** argv)
{
    ros::init(argc, argv, "fanuc_circle");
    ros::NodeHandle node_handle;

    ros::AsyncSpinner spinner(1);
    spinner.start();


    // Create the interface to establish the communication with move_group
    static const std::string PLANNING_GROUP = "fanuc_arm";
    moveit::planning_interface::MoveGroupInterface move_group(PLANNING_GROUP);

    // Creation of the joint model group
    const robot_state::JointModelGroup* joint_model_group =
      move_group.getCurrentState()->getJointModelGroup(PLANNING_GROUP);
    
    // Initialization of the moveit visual tool used to visualize the markers on rviz 
    namespace rvt = rviz_visual_tools;
    moveit_visual_tools::MoveItVisualTools visual_tools("base_link");

    // Printing of the Basic Information
    ROS_INFO_NAMED("tutorial", "Planning frame: %s", move_group.getPlanningFrame().c_str());
    ROS_INFO_NAMED("tutorial", "End effector link: %s", move_group.getEndEffectorLink().c_str());
    ROS_INFO_NAMED("tutorial", "Available Planning Groups:");

    visual_tools.prompt("Press 'next' in the RvizVisualToolsGui window to start the demo");

    /*
        The following is used to define the path waypoints
    */
    std::vector<geometry_msgs::Pose> waypoints;

    // Set the home_pose as the initial flange pose as specified in the urdf file
    geometry_msgs::Pose home_pose = move_group.getCurrentPose("flange").pose;

    // Robot Model creation
    robot_model_loader::RobotModelLoaderConstPtr robot_model_loader(new robot_model_loader::RobotModelLoader("robot_description"));
    robot_model::RobotModelConstPtr kinematic_model = robot_model_loader->getModel();

    // Path parameters (circle)
    int angle_resolution = 5;
    double d_angle = angle_resolution*3.14/180;
    double angle= 0;
    double radius = 0.3;
    double y_center = home_pose.position.y;
    double z_center = home_pose.position.z - radius;
    double x_center = home_pose.position.x;

    // Creation of an orientation constraint 
    moveit_msgs::OrientationConstraint ocm;
    ocm.link_name = "flange";
    ocm.header.frame_id = "base_link";
    ocm.absolute_x_axis_tolerance = 0.1;
    ocm.absolute_y_axis_tolerance = 0.1;
    ocm.absolute_z_axis_tolerance = 0.1;
    ocm.weight = 1.0;

    moveit_msgs::Constraints test_constraints;
    test_constraints.orientation_constraints.push_back(ocm);
    move_group.setPathConstraints(test_constraints);

    // Initilize the orientation of each path point 
    geometry_msgs::Pose pathPoint;
    pathPoint.orientation.x = home_pose.orientation.x;
    pathPoint.orientation.y = home_pose.orientation.y;
    pathPoint.orientation.z = home_pose.orientation.z;
    pathPoint.position.z = home_pose.position.z;
    pathPoint.position.y = home_pose.position.y;
    pathPoint.position.x = home_pose.position.x;
    
    // Planning the path
    waypoints.push_back(pathPoint);
    for (int i= 0; i< (360/angle_resolution); i++)
    {
        pathPoint.position.z = z_center + radius*cos(angle);
        pathPoint.position.y = y_center + radius*sin(angle);
        pathPoint.position.x = x_center;

        waypoints.push_back(pathPoint);
        angle+= d_angle;
    }
    waypoints.push_back(home_pose);

    ROS_INFO("There are %d number of waypoints", (int)(waypoints.size()));
    
    /* 
        We want the Cartesian path to be interpolated at a resolution of 3 cm 
        which is why we will specify 0.03 as the max step in Cartesian translation. 
        We will specify the jump threshold as 0.0, effectively disabling it.
    */
    double eef_resolution = 0.03;
    double jump_threshold = 0.0;
    moveit_msgs::RobotTrajectory robot_trajectory_msg;
    double fraction = move_group.computeCartesianPath(waypoints, eef_resolution, jump_threshold, robot_trajectory_msg);

    ROS_INFO("Percentage acheived: %.2f%%",  fraction * 100.0);

    // Visualize the plan in RViz
    visual_tools.publishPath(waypoints, rvt::RED, rvt::SMALL);
    visual_tools.trigger();

    visual_tools.prompt("Press 'next' in the RvizVisualToolsGui window to publish the joint space solution");
    
    /*
        Adding velocity and acceleration constraints to the computed trajectory
    */
    robot_trajectory::RobotTrajectory robot_trajectory(kinematic_model, joint_model_group);

    double dt = 0;
    Eigen::VectorXd joint_positions_prev = Eigen::VectorXd::Zero(joint_model_group->getVariableCount());
    Eigen::VectorXd joint_positions_curr = Eigen::VectorXd::Zero(joint_model_group->getVariableCount());
    Eigen::VectorXd joint_velocities_prev = Eigen::VectorXd::Zero(joint_model_group->getVariableCount());
    Eigen::VectorXd joint_velocities_curr = Eigen::VectorXd::Zero(joint_model_group->getVariableCount());
    Eigen::VectorXd joint_accelerations = Eigen::VectorXd::Zero(joint_model_group->getVariableCount());

    ROS_WARN_STREAM("n. points: " << robot_trajectory_msg.joint_trajectory.points.size());
    
    // Robot state creation
    moveit::core::RobotState robot_state(kinematic_model);

    // Computing the IK solution for each waypoint and applying the valocity and acceleration constraint 
    for(int i=0; i < robot_trajectory_msg.joint_trajectory.points.size(); i++)
    {

        if(!robot_state.setFromIK(joint_model_group, waypoints[i]))
            ROS_WARN_STREAM("Could not compute IK solution for waypoint " << i);

        if(i > 0)
        {
            double tfs_curr = robot_trajectory_msg.joint_trajectory.points[i].time_from_start.toSec();
            double tfs_prev = robot_trajectory_msg.joint_trajectory.points[i-1].time_from_start.toSec();
            dt = tfs_curr - tfs_prev;

            robot_state.copyJointGroupPositions(joint_model_group, joint_positions_curr);

            computeVectorsDifference(joint_velocities_curr, joint_positions_curr, joint_positions_prev, joint_model_group);
            joint_velocities_curr = joint_velocities_curr/dt;

            joint_accelerations = (joint_velocities_curr - joint_velocities_prev)/dt;

            robot_state.setJointGroupVelocities(joint_model_group, joint_velocities_curr);
            robot_state.setJointGroupAccelerations(joint_model_group, joint_accelerations);
        }
        
        // Add a point to the trajectory by relating a robot state to each time instant
        robot_trajectory.addSuffixWayPoint(robot_state, dt);

        joint_positions_prev = joint_positions_curr;
        joint_velocities_prev = joint_velocities_curr;
    }

    // Converting the trajectory object to a trajecotry message
    robot_trajectory.getRobotTrajectoryMsg(robot_trajectory_msg);

    // Prepare display trajectory message and publish it
    ros::Publisher display_path_publisher = node_handle.advertise<moveit_msgs::DisplayTrajectory>(planning_pipeline::PlanningPipeline::DISPLAY_PATH_TOPIC, 10, true);
    moveit_msgs::DisplayTrajectory display_trajectory_msg;
    robot_state::robotStateToRobotStateMsg(robot_state, display_trajectory_msg.trajectory_start);
    display_trajectory_msg.model_id = kinematic_model->getName();
    display_trajectory_msg.trajectory.resize(1, robot_trajectory_msg);
    display_path_publisher.publish(display_trajectory_msg);
    
    visual_tools.prompt("Press 'next' in the RvizVisualToolsGui window to execute the trajectory");
    
    // Execution of the trajectory in rviz
    move_group.execute(robot_trajectory_msg);

    // Publish joint space solution to plot in rqt_multiplot
    ros::Publisher plot_trajectory_publisher = node_handle.advertise<trajectory_msgs::JointTrajectoryPoint>("plot_planned_trajectory", 10000, true);
    
    ros::Duration sleep_time(0.05);
    
    for(int i=0; i < robot_trajectory_msg.joint_trajectory.points.size(); i++)
    {
        trajectory_msgs::JointTrajectoryPoint jtp = robot_trajectory_msg.joint_trajectory.points[i];
        plot_trajectory_publisher.publish(jtp);
        sleep_time.sleep();
    }

    visual_tools.prompt("Press 'next' in the RvizVisualToolsGui window to terminate the planner");

    spinner.stop();
    ros::shutdown();
    exit(0);

}

// Utility function
void computeVectorsDifference(
    Eigen::VectorXd & diff,
    const Eigen::VectorXd & minuend,
    const Eigen::VectorXd & subtrahend,
    const moveit::core::JointModelGroup * jmg)
{
    for (int i = 0; i < minuend.size(); i++)
    {
        if (jmg->getActiveJointModels()[i]->getType() == robot_model::JointModel::REVOLUTE)
        {
            // Compute difference between revolute joints
            robot_model::VariableBounds bounds = jmg->getParentModel().getVariableBounds(jmg->getVariableNames()[i]);

            if(bounds.position_bounded_)
            {
                angles::shortest_angular_distance_with_limits(
                    angles::normalize_angle(subtrahend[i]), 
                    angles::normalize_angle(minuend[i]), 
                    angles::normalize_angle(bounds.min_position_), 
                    angles::normalize_angle(bounds.max_position_), 
                    diff[i]);
            }
            else
                diff[i] = angles::shortest_angular_distance(subtrahend[i], minuend[i]);
        }
        else if (jmg->getActiveJointModels()[i]->getType() == robot_model::JointModel::PLANAR)
        {
            ROS_ERROR("Planar joints are not currently supported.");
        }
        else
        {
            // ! Other joint model types are included here (PRISMATIC, FIXED, UNKNOWN, etc.)
            diff[i] = minuend[i] - subtrahend[i];
        }
    }
}