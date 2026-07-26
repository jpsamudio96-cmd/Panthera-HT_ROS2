#ifndef PANTHERA_MOVEIT_ADAPTER__MOVEIT_ADAPTER_NODE_HPP_
#define PANTHERA_MOVEIT_ADAPTER__MOVEIT_ADAPTER_NODE_HPP_

#include <rclcpp/rclcpp.hpp>

#include <thread>

#include <rclcpp/executors/single_threaded_executor.hpp>

#include "panthera_interfaces/msg/detected_object.hpp"

#include <moveit/move_group_interface/move_group_interface.h>

#include <geometry_msgs/msg/pose.hpp>

using MoveGroupInterface =
    moveit::planning_interface::MoveGroupInterface;

    
class MoveItAdapter
{
public:
    explicit MoveItAdapter(
        const rclcpp::Node::SharedPtr& node
    );

    void initializeMoveGroup();

private:

    void targetCallback(
        const panthera_interfaces::msg::DetectedObject::SharedPtr msg
    );

    geometry_msgs::msg::Pose buildTargetPose(
        const panthera_interfaces::msg::DetectedObject & object
    );

    bool validateTargetPose(
        const geometry_msgs::msg::Pose& pose
    );

    bool prepareMotionPlan(
        const geometry_msgs::msg::Pose& target_pose
    );

    geometry_msgs::msg::Pose target_pose_;

    std::shared_ptr<MoveGroupInterface> arm_;

    rclcpp::Subscription<
        panthera_interfaces::msg::DetectedObject
    >::SharedPtr target_subscription_;

    rclcpp::Node::SharedPtr node_;

    //
    // Dedicated MoveIt node
    //

    rclcpp::Node::SharedPtr moveit_node_;

    rclcpp::executors::SingleThreadedExecutor moveit_executor_;

    std::thread moveit_spin_thread_;


    // 
    bool planning_requested_;

    MoveGroupInterface::Plan current_plan_;

};

#endif  // PANTHERA_MOVEIT_ADAPTER__MOVEIT_ADAPTER_NODE_HPP_

