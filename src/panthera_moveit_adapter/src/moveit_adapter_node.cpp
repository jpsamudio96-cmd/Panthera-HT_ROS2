#include "panthera_moveit_adapter/moveit_adapter_node.hpp"

#include <functional>

MoveItAdapterNode::MoveItAdapterNode()
    : Node("panthera_moveit_adapter")
{
    detected_object_subscription_ =
        this->create_subscription<
            panthera_interfaces::msg::DetectedObject
        >(
            "/perception/target_object",
            10,
            std::bind(
                &MoveItAdapterNode::detectedObjectCallback,
                this,
                std::placeholders::_1
            )
        );

    RCLCPP_INFO(
        this->get_logger(),
        "Panthera MoveIt Adapter initialized."
    );
}

void MoveItAdapterNode::detectedObjectCallback(
    const panthera_interfaces::msg::DetectedObject::SharedPtr msg
)
{
    RCLCPP_INFO(
        this->get_logger(),
        "Detected object received."
    );

    RCLCPP_INFO(
        this->get_logger(),
        "Class: %s",
        msg->class_name.c_str()
    );

    RCLCPP_INFO(
        this->get_logger(),
        "Confidence: %.2f",
        msg->confidence
    );

    RCLCPP_INFO(
        this->get_logger(),
        "Workspace Position: [%.3f, %.3f, %.3f]",
        msg->workspace_x,
        msg->workspace_y,
        msg->workspace_z
    );

    RCLCPP_INFO(
        this->get_logger(),
        "Centroid: (%u, %u)",
        msg->centroid_u,
        msg->centroid_v
    );

    RCLCPP_INFO(
        this->get_logger(),
        "Theta: %.2f deg",
        msg->theta
    );
}