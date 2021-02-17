#include <actionlib/server/simple_action_server.h>
#include <kinematics_action/inverse_kinematic_action.h>
#include <moveit/robot_state/conversions.h>
#include <angles/angles.h>
#include <moveit/kinematics_base/kinematics_base.h>
#include <moveit_msgs/MoveItErrorCodes.h>


// Constructor
kinematics_action::InverseKinematicsAction::InverseKinematicsAction():
    action_server_(nh_,"ik_solver", boost::bind(&kinematics_action::InverseKinematicsAction::ik_callback_, this, _1), false),
    robot_model_loader_(new robot_model_loader::RobotModelLoader("robot_description")), 
    kinematic_model_(robot_model_loader_->getModel()),
    jmg_(nullptr)
{
    std::string planning_group_name;
    if(!nh_.getParam("planning_group_name", planning_group_name))
    {
        ROS_ERROR("'planning_group_name' is undefined on the parameter server");
        return;
    }

    jmg_ = kinematic_model_-> getJointModelGroup(planning_group_name);
    // Start the action server
    action_server_.start();

}

// Deconstructor
kinematics_action::InverseKinematicsAction::~InverseKinematicsAction(){

}

// IK computation
void kinematics_action::InverseKinematicsAction::ik_callback_(const kinematics_action_msgs::GetIKSolutionsGoalConstPtr & goal)
{
    ROS_INFO("IK solver: goal received");

    // Get Instance of KDL solver (as specified in moveit_config)
    const kinematics::KinematicsBaseConstPtr solver = jmg_->getSolverInstance();
    kinematics_action_msgs::GetIKSolutionsResult result;
    int ik_calls_counter = 0;

    while(ik_calls_counter < 2000 && ros::ok()){
        
        // Generate a seed to always start from a different point in the IK computation
        std::vector<double> seed_state = generateSeedState_();
        std::vector<double> solution;
        moveit_msgs::MoveItErrorCodes error_code;

        // Compute IK solution using KDL solver
        solver->getPositionIK(goal->end_effector_pose, seed_state, solution, error_code);
        
        // Check if a solution was found
        if(error_code.val == moveit_msgs::MoveItErrorCodes::SUCCESS){
            // Normalize the solution of the revolute joint angles in order to avoid the acceptance of an already considered solution
            normalizeJointPositions_(solution);
            
            // Check if the solution is new
            if(isSolutionNew_(solution)){
                
                // Add the solution to the response
                ik_solutions_.push_back(solution);

                // Get the RobotState from the RobotModel
                moveit::core::RobotState robot_state(kinematic_model_);
                
                // Copy the position values into the state
                robot_state.setVariablePositions(solution);

                // Convert the RobotState to a RobotState message, publish a feedback and add the solution to result
                kinematics_action_msgs::GetIKSolutionsFeedback feedback;
                moveit::core::robotStateToRobotStateMsg(robot_state, feedback.ik_solution);
                action_server_.publishFeedback(feedback);
                result.ik_solutions.push_back(feedback.ik_solution);
            }
        }
        ik_calls_counter++;
    }
    // If no solution were founded set the status of the active goal to aborted else set the status to succeeded and return the result
    if(ik_solutions_.size() == 0){
        action_server_.setAborted(result, "could not find any IK solution");
    }
    else{
        std::ostringstream ss;
        ss << "found " << ik_solutions_.size() << " IK Solutons";
        action_server_.setSucceeded(result, ss.str());
    }
    ik_solutions_.resize(0);
}
 

bool kinematics_action::InverseKinematicsAction::isSolutionNew_(const std::vector<double> & solution) const{
    for (int i= 0; i<ik_solutions_.size(); i++){
        bool are_solutions_equal = true;

        for(int j= 0; j< ik_solutions_[i].size() && are_solutions_equal; j++){
            double diff;
            if(jmg_->getActiveJointModels()[j]-> getType() == robot_model::JointModel::REVOLUTE)
                diff = angles::shortest_angular_distance(ik_solutions_[i][j], solution[j]);
            else 
                diff = ik_solutions_[i][j] - solution[j];
            
            if(std::fabs(diff)>1e-3)
                are_solutions_equal = false;
        }
        if(are_solutions_equal)
            return false;
    }

    return true;
}

std::vector<double> kinematics_action::InverseKinematicsAction::generateSeedState_() const{
    std::vector<double> seed_state;
    std::vector<std::string> joint_names = kinematic_model_->getVariableNames();
    for(int i = 0; i<joint_names.size(); i++){
        double lb = kinematic_model_->getURDF()->getJoint(joint_names[i])->limits->lower;
        double ub = kinematic_model_->getURDF()->getJoint(joint_names[i])->limits->upper;
        double span = ub-lb;
        seed_state.push_back((double)std::rand()/RAND_MAX*span + lb);
    }
    return seed_state;
}

// normalize the angles of the revolute joints in order to 
void kinematics_action::InverseKinematicsAction::normalizeJointPositions_(std::vector<double> & solution) const{
    for(int i=0;i<solution.size();i++){
        if(jmg_->getActiveJointModels()[i]->getType() == robot_model::JointModel::REVOLUTE)
            solution[i] = angles::normalize_angle(solution[i]);
    }
}







