#ifndef PANTHERA_MOVEIT_ADAPTER__MOVEIT_ADAPTER_NODE_HPP_
#define PANTHERA_MOVEIT_ADAPTER__MOVEIT_ADAPTER_NODE_HPP_

#include <memory>

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

    void executeCupRoutine();

    void detectedObjectCallback(
        const panthera_interfaces::msg::DetectedObject::SharedPtr msg
    );

    std::shared_ptr<MoveGroupInterface> arm_;

    rclcpp::Subscription<
        panthera_interfaces::msg::DetectedObject
    >::SharedPtr detected_object_subscription_;

    bool robot_busy_;
};

#endif