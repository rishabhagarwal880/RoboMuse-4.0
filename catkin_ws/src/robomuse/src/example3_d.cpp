#include <ros/ros.h>
    #include <tf/transform_broadcaster.h>
    #include <nav_msgs/Odometry.h>
#include"robomuse/chapter2_msg1.h"
     double x = 0.0;
     double y = 0.0;
     double th = 0.0;
     double vx ;
     double vy ;
     double vth ;
     double Dl;
     double Dr;
     double Distancel,Distancer,LastDistancel,LastDistancer;
double L=0.558;
 void messageCallback(const robomuse::chapter2_msg1::ConstPtr& msg)
{
    Distancel=msg->A;
Distancer=msg->B;
  Distancel=Distancel/2048*2*3.14*.12;
  Distancer=Distancer/2048*2*3.14*.12;

}   
    int main(int argc, char** argv){
      ros::init(argc, argv, "odometry_publisher");
      ros::NodeHandle nh;
ros::Subscriber sub = nh.subscribe("message", 1000, messageCallback);
      ros::NodeHandle n;
      ros::Publisher odom_pub = n.advertise<nav_msgs::Odometry>("odom", 50);
     tf::TransformBroadcaster odom_broadcaster;
   
   
     ros::Time current_time, last_time;
     current_time = ros::Time::now();
     last_time = ros::Time::now();
   
     ros::Rate r(1.0);
     while(n.ok())
{     
       ros::spinOnce();               // check for incoming messages
       double dt = (current_time - last_time).toSec();
        Dl=Distancel-LastDistancel;
        Dr=Distancer-LastDistancer;
        LastDistancel=Distancel;
        LastDistancer=Distancer;

        double Dc=0;
	Dc=(Dl+Dr)/2; 
      current_time = ros::Time::now();
   
       //compute odometry in a typical way given the velocities of the root
       	double delta_x=Dc*cos(th);
	double delta_y=Dc*sin(th);
	double delta_th=(Dr-Dl)/L; 
       x += delta_x;
       y += delta_y;
       th += delta_th;
   	vx=delta_x/dt;
	vy=delta_y/dt;
	vth=delta_th/dt;
   
       //since all odometry is 6DOF we'll need a quaternion created from yaw
ROS_INFO("y: [%f]", y);
ROS_INFO("x: [%f]", x);
       geometry_msgs::Quaternion odom_quat = tf::createQuaternionMsgFromYaw(th);
   
       //first, we'll publish the transform over tf
      geometry_msgs::TransformStamped odom_trans;
       odom_trans.header.stamp = current_time;
       odom_trans.header.frame_id = "odom";
       odom_trans.child_frame_id = "base_link";
  
       odom_trans.transform.translation.x = x;
       odom_trans.transform.translation.y = y;
       odom_trans.transform.translation.z = 0.0;
       odom_trans.transform.rotation = odom_quat;
   
       //send the transform
       odom_broadcaster.sendTransform(odom_trans);
   
       //next, we'll publish the odometry message over ROS
       nav_msgs::Odometry odom;
       odom.header.stamp = current_time;
       odom.header.frame_id = "odom";
   
       //set the position
       odom.pose.pose.position.x = x;
       odom.pose.pose.position.y = y;
       odom.pose.pose.position.z = 0.0;
       odom.pose.pose.orientation = odom_quat;
   
      //set the velocity
       odom.child_frame_id = "base_link";
       odom.twist.twist.linear.x = vx;
       odom.twist.twist.linear.y = vy;
      odom.twist.twist.angular.z = vth;
   
       //publish the message
       odom_pub.publish(odom);
   
       last_time = current_time;
       r.sleep();
     }
      
   }
