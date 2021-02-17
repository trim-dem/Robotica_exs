# ESERCITAZIONI

## ESERCITAZIONE 2:
* Execute the following steps:
    * Position yourself in the workspace folder:
        * cd ex1_ws
    * Setup the project:
        * catkin clean
        * catkin build
        * source devel/setup.bash
    * Launch the simulation:
        * roslaunch src/ex2_pkg/launch/ex2.launch

## ESERCITAZIONE 3:
* Execute the following steps:
    * Position yourself in the workspace folder:
        * cd fanuc_ws
    * Setup the project:
        * catkin clean
        * catkin build
        * source devel/setup.bash
    * Visualize the robot in RViz:
        * roslaunch urdf_tutorial display.launch model:=src/fanuc_description/robot/fanuc.urdf
    * Visualize the robot in Gazebo:
        * roslaunch src/fanuc_moveit_config/launch/demo.launch
    * Print the TF of the end-effector in all the reference frames of all joints:
        * rosrun fanuc_description allToFlange

## ESERCITAZIONE 4:
* Execute the following steps:
    * Position yourself in the workspace folder:
        * cd fanuc_ws
    * Setup the project:
        * catkin clean
        * catkin build
        * source devel/setup.bash
    * Visualize the robot in RViz and execute the service server:
        * roslaunch fanuc_description ex4_fk.launch
    * Execute the service client:
        * rosrun fanuc_description fk_client
    * Visualize the robot in RViz and execute the action server:
        * roslaunch kinematics_action ex4_ik.launch
    * Execute the action client:
        * rosrun kinematics_action client
    * Additionaly, you can check what happens after removing the joints angle limits using the following commands:
        * roslaunch kinematics_action ex4_ik.launch urdf_name:=fanuc_no_limit
        * rosrun kinematics_action client

## ESERCITAZIONE 5:
* Execute the following steps:
    * Position yourself in the workspace folder:
        * cd Exercise5
    * Setup the project:
        * catkin clean
        * catkin build
        * source devel/setup.bash
    * Visualize the robot in RViz, execute the circle trajectory and monitor the joints position,
    * velocity and acceleration (use the "next" button in the rviz to perform the steps):
        * roslaunch fanuc_move_group_interface ex5.launch
