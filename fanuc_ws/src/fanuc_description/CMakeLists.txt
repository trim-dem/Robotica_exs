cmake_minimum_required(VERSION 2.8.3)

project(fanuc_description)

find_package(catkin REQUIRED COMPONENTS
	roscpp
	tf2_ros
	geometry_msgs
	moveit_msgs
	moveit_ros_planning_interface
	eigen_conversions
)

include_directories(${catkin_INCLUDE_DIRS})

catkin_package(
	  CATKIN_DEPENDS 
	  roscpp
	  tf2_ros
	  geometry_msgs
	  moveit_msgs
	  moveit_ros_planning_interface
	  eigen_conversions
)

add_executable(allToFlange nodes/allToFlange.cpp)
add_executable(fk_client services/fk_client.cpp)
add_executable(fk_server services/fk_server.cpp)

target_link_libraries(allToFlange
${catkin_LIBRARIES}
)

target_link_libraries(fk_client
${catkin_LIBRARIES}
)

target_link_libraries(fk_server
${catkin_LIBRARIES}
)

foreach(dir meshes robot)
	install(DIRECTORY ${dir}/
		DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}/${dir})
endforeach(dir)
