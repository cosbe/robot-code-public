#!/bin/sh
#
# This script runs vision and all the things vision needs:
# * Set camera parameters - minimum exposure and brightness
# * Run vision
#
# Usage: startup.sh roborio-ip camera-index
#
# To run this on startup, add the following line or something similar to /etc/rc.init
#
#     /home/ubuntu/robot-code/c2017/vision/coprocessor/startup.sh 10.16.78.22 >& /home/ubuntu/log.text &
#

# This script runs before the camera is ready, so sleep until it is ready.
sleep 30s
v4l2ctrl -d /dev/video$2 -l c2017/vision/coprocessor/camera_params
echo Robot ip: $1
echo Camera index: $2
cd /home/ubuntu/robot-code
./bazel-bin/c2017/vision/coprocessor/main --robot_ip=$1 --camera_index=$2
