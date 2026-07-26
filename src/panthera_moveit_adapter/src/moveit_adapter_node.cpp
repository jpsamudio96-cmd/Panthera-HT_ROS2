#include "panthera_moveit_adapter/moveit_adapter_node.hpp"

#include <functional>

MoveItAdapterNode::MoveItAdapterNode()
    : Node("panthera_moveit_adapter"),
      robot_busy_(false)
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
        get_logger(),
        "Panthera MoveIt Adapter initialized."
    );
}

void MoveItAdapterNode::initializeMoveIt()
{
    arm_ = std::make_shared<MoveGroupInterface>(
        shared_from_this(),
        "arm"
    );

    arm_->setMaxVelocityScalingFactor(1.0);
    arm_->setMaxAccelerationScalingFactor(1.0);

    RCLCPP_INFO(
        get_logger(),
        "MoveGroupInterface initialized."
    );
}

void MoveItAdapterNode::executeCupRoutine()
{
    RCLCPP_INFO(
        get_logger(),
        "Executing CUP routine..."
    );
}

void MoveItAdapterNode::detectedObjectCallback(
    const panthera_interfaces::msg::DetectedObject::SharedPtr msg
)
{
    if (robot_busy_)
    {
        return;
    }

    robot_busy_ = true;

    if (!arm_)
    {
        initializeMoveIt();
    }

    RCLCPP_INFO(
        get_logger(),
        "Detected object received."
    );

    RCLCPP_INFO(
        get_logger(),
        "Class: %s",
        msg->class_name.c_str()
    );

    RCLCPP_INFO(
        get_logger(),
        "Confidence: %.2f",
        msg->confidence
    );

    RCLCPP_INFO(
        get_logger(),
        "Workspace Position: [%.3f, %.3f, %.3f]",
        msg->workspace_x,
        msg->workspace_y,
        msg->workspace_z
    );

    RCLCPP_INFO(
        get_logger(),
        "Centroid: (%u, %u)",
        msg->centroid_u,
        msg->centroid_v
    );

    RCLCPP_INFO(
        get_logger(),
        "Theta: %.2f deg",
        msg->theta
    );

    if (msg->class_name == "cup")
    {
        executeCupRoutine();
    }

    robot_busy_ = false;
}