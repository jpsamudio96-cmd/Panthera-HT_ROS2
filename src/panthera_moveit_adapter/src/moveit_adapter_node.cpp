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
    if (arm_)
    {
        return;
    }

    arm_ = std::make_shared<MoveGroupInterface>(
        shared_from_this(),
        "arm"
    );

    arm_->setMaxVelocityScalingFactor(0.30);
    arm_->setMaxAccelerationScalingFactor(0.30);

    RCLCPP_INFO(
        get_logger(),
        "MoveGroupInterface initialized."
    );
}

void MoveItAdapterNode::executeCupRoutine()
{
    if (!arm_)
    {
        RCLCPP_ERROR(
            get_logger(),
            "MoveGroupInterface not initialized."
        );
        return;
    }

    RCLCPP_INFO(
        get_logger(),
        "Planning to named target: pose1"
    );

    arm_->setStartStateToCurrentState();

    arm_->setNamedTarget("pose1");

    MoveGroupInterface::Plan plan;

    auto result = arm_->plan(plan);

    if (result != moveit::core::MoveItErrorCode::SUCCESS)
    {
        RCLCPP_ERROR(
            get_logger(),
            "Planning failed."
        );
        return;
    }

    RCLCPP_INFO(
        get_logger(),
        "Planning succeeded."
    );

    result = arm_->execute(plan);

    if (result == moveit::core::MoveItErrorCode::SUCCESS)
    {
        RCLCPP_INFO(
            get_logger(),
            "Execution completed."
        );
    }
    else
    {
        RCLCPP_ERROR(
            get_logger(),
            "Execution failed."
        );
    }
}

void MoveItAdapterNode::detectedObjectCallback(
    const panthera_interfaces::msg::DetectedObject::SharedPtr msg
)
{
    if (!arm_)
    {
        initializeMoveIt();
    }

    if (robot_busy_)
    {
        return;
    }

    if (msg->class_name != "cup")
    {
        return;
    }

    robot_busy_ = true;

    RCLCPP_INFO(
        get_logger(),
        "Detected cup."
    );

    executeCupRoutine();

    robot_busy_ = false;
}