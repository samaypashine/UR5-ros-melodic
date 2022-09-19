
#include "ros/ros.h"
#include "std_msgs/String.h"
#include "sensor_msgs/Image.h"

#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>
#include <sensor_msgs/image_encodings.h>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

#include <string>
#include <fstream>
#include <iostream> 
#include <cstdlib>
#include "joint_recorder/recorderMsg.h"
#include <chrono>
#include <ctime>

#include <sstream>
#include <librealsense2/rs.hpp>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include<stdlib.h>
#include<string.h>




bool recordFlag = false;
std::ofstream file; 
std::string folderName, thisNodeName;
// int num1 = 1;
int num2 = 1;
// int num3 = 1;


void frame_topic2_callback(const sensor_msgs::ImageConstPtr& msg)
{
    if (recordFlag)
    {
        ROS_INFO("Inside topic2 callback");
        cv_bridge::CvImagePtr cv_ptr;
        try
        {
          cv_ptr = cv_bridge::toCvCopy(msg);
        }
        catch (cv_bridge::Exception& e)
        {
          ROS_WARN("INSIDE topic2 callback");
          ROS_ERROR("cv_bridge exception: %s", e.what());
          return;
        }

        std::string png_file = folderName + "image-" + std::to_string(num2) + ".jpg";  
        cv::imwrite(png_file, cv_ptr->image);

        // std::string command = "python3 /home/samay/catkin_ws/src/digit/src/recordTouchData.py --num " + std::to_string(num3);
        // system(command.c_str());
        num2++;
    }
    ros::spinOnce();
}



void recordingControlCallback(const joint_recorder::recorderMsg::ConstPtr& msg)
{
    if((msg->command.data.compare("set_file_name") == 0) && (msg->topic.data.compare(thisNodeName) == 0))
    {
      ROS_INFO("Setted the File name.");
      folderName = msg->fileName.data;
    }
    
    else if(!folderName.empty())
    { 
      if (msg->command.data.compare("start") == 0)
      { 
        ROS_INFO("In Start condition of recordService callback function");
        recordFlag = true;
      }
      
      else if (msg->command.data.compare("stop") == 0)
      {
          ROS_ERROR("In STOP condition of recordService callback function"); //Stop recording but keep node running
          recordFlag = false;
          // num1 = 1;
          num2 = 1;
          // num3 = 1;
          folderName = "";
      }
      
      else if (msg->command.data.compare("shutdown") == 0) //shut node down
      {
          ROS_ERROR("Shutting down");
          ros::shutdown();
      }
      
      else
      {
        ROS_ERROR("Command Should be \"set_file_name\" \"start\" or \"stop\" or or \"shutdown\"");
      }
    }

    else("File name needs to be set before sending command to start recording");
}



int main(int argc, char **argv)
{
  ros::init(argc, argv, "depth_frame_recorder_node");
  thisNodeName = "depth_frame_capture";

  ros::NodeHandle n;

  ros::Subscriber controlSub = n.subscribe("recording_control_topic", 10, recordingControlCallback);
  ros::Subscriber depth_sub = n.subscribe("/camera/depth/image_rect_raw", 10, &frame_topic2_callback);

  while(ros::ok())
  {
    ROS_INFO("Listening to /camera/depth/image_rect_raw");
    ros::spinOnce();
  }

  return 0;

}