#!/bin/bash

killall gzserver
killall gzclient
killall rviz
killall roscore
killall rosmaster

#1. launch roscore
#2. load parameters 
#3. generate world
#4. launch world 

echo "Running tour: $1"

roscore -p $ROSPORT&

until rostopic list; do sleep 0.5; done #wait until rosmaster has started 

rosparam load src/jackal_velodyne/params/simulation_params.yaml
rosparam load src/myhal_simulator/params/common_vehicle_params.yaml
rosparam load src/myhal_simulator/params/animation_params.yaml
rosparam load src/myhal_simulator/params/room_params_V2.yaml
rosparam load src/myhal_simulator/params/scenario_params_V2.yaml
rosparam load src/myhal_simulator/params/plugin_params.yaml
rosparam load src/myhal_simulator/params/model_params.yaml
rosparam set use_sim_time true
rosparam set bag_name $1
t=$(date +'%Y-%m-%d-%H-%M-%S')
rosparam set start_time $t
mkdir "/home/$USER/Myhal_Simulation/simulated_runs/$t"
touch "/home/$USER/Myhal_Simulation/simulated_runs/$t/log.txt"

sleep 0.1

rosrun myhal_simulator world_factory

rosrun jackal_velodyne diagnostics &
roslaunch jackal_velodyne master.launch &
rosbag record -O "/home/$USER/Myhal_Simulation/simulated_runs/$t/raw_data.bag" -a -x "/kinect_V2(.*)" # Limiting data to remain under rosbag buffer




