#ifndef PANTHERA_MOVEIT_ADAPTER__MOVEIT_ADAPTER_NODE_HPP_
#define PANTHERA_MOVEIT_ADAPTER__MOVEIT_ADAPTER_NODE_HPP_

#include <rclcpp/rclcpp.hpp>

#include "panthera_interfaces/msg/detected_object.hpp"

class MoveItAdapterNode : public rclcpp::Node
{
public:
    MoveItAdapterNode();

private:
    void detectedObjectCallback(
        const panthera_interfaces::msg::DetectedObject::SharedPtr msg
    );

    rclcpp::Subscription<
        panthera_interfaces::msg::DetectedObject
    >::SharedPtr detected_object_subscription_;
};

#endif  // PANTHERA_MOVEIT_ADAPTER__MOVEIT_ADAPTER_NODE_HPP_