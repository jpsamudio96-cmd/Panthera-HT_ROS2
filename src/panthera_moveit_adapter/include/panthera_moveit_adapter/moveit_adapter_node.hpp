#ifndef PANTHERA_MOVEIT_ADAPTER__MOVEIT_ADAPTER_NODE_HPP_
#define PANTHERA_MOVEIT_ADAPTER__MOVEIT_ADAPTER_NODE_HPP_

#include <memory>
#include <string>

#include <chrono>

#include <rclcpp/rclcpp.hpp>

#include "moveit/move_group_interface/move_group_interface.h"
#include "panthera_interfaces/msg/detected_object.hpp"

class MoveItAdapterNode : public rclcpp::Node
{
public:
    MoveItAdapterNode();

private:

    using MoveGroupInterface =
        moveit::planning_interface::MoveGroupInterface;

    void initializeMoveIt();

    bool executeNamedTarget(
        const std::string& target_name
    );

    bool executeGripperTarget(
        const std::string& target_name
    );

    bool dispatchObject(
        const std::string& object_name
    );

    void executeCupRoutine();

    void executeBottleRoutine();

    void detectedObjectCallback(
        const panthera_interfaces::msg::DetectedObject::SharedPtr msg
    );

    std::shared_ptr<MoveGroupInterface> arm_;

    std::shared_ptr<MoveGroupInterface> gripper_;

    rclcpp::Subscription<
        panthera_interfaces::msg::DetectedObject
    >::SharedPtr detected_object_subscription_;

    bool robot_busy_;
};

#endif  // PANTHERA_MOVEIT_ADAPTER__MOVEIT_ADAPTER_NODE_HPP_