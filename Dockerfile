FROM ros:noetic-robot-focal

RUN apt-get update && apt-get install -y python3-catkin-tools ros-noetic-octomap-msgs ros-noetic-octomap ros-noetic-ompl ros-noetic-octomap-ros ros-noetic-lms1xx ros-noetic-ifopt

RUN mkdir -p /home/vkc
COPY src /home/vkc/src
RUN . /opt/ros/noetic/setup.sh
RUN cd /home/vkc && DEBIAN_FRONTEND=noninteractive rosdep install --from-paths src --ignore-src -r -y

RUN cd /home/vkc && catkin config --extend /opt/ros/noetic && catkin build
