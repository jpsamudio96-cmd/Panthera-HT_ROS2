#include "panthera_moveit_adapter/moveit_adapter_node.hpp"

#include <functional>

MoveItAdapterNode::MoveItAdapterNode()
    : Node("panthera_moveit_adapter"),
        robot_busy_(false),
        cup_detected_(false)
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

bool MoveItAdapterNode::executeNamedTarget(
    const std::string& target_name
)
{
    arm_->setStartStateToCurrentState();

    arm_->setNamedTarget(target_name);

    MoveGroupInterface::Plan plan;

    auto result = arm_->plan(plan);

    if (result != moveit::core::MoveItErrorCode::SUCCESS)
    {
        RCLCPP_ERROR(
            get_logger(),
            "Planning failed for target '%s'.",
            target_name.c_str()
        );
        return false;
    }

    RCLCPP_INFO(
        get_logger(),
        "Executing '%s'...",
        target_name.c_str()
    );

    result = arm_->execute(plan);

    if (result != moveit::core::MoveItErrorCode::SUCCESS)
    {
        RCLCPP_ERROR(
            get_logger(),
            "Execution failed for target '%s'.",
            target_name.c_str()
        );
        return false;
    }

    RCLCPP_INFO(
        get_logger(),
        "Target '%s' completed.",
        target_name.c_str()
    );

    return true;
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

    if (!executeNamedTarget("pose1"))
    {
        return;
    }

    rclcpp::sleep_for(
        std::chrono::seconds(1)
    );

    executeNamedTarget("home");
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

    // Si deja de verse una taza,
    // el sistema queda listo para un nuevo disparo.
    if (msg->class_name != "cup")
    {
        cup_detected_ = false;
        return;
    }

    // La taza sigue siendo la misma.
    if (cup_detected_)
    {
        return;
    }

    cup_detected_ = true;

    robot_busy_ = true;

    RCLCPP_INFO(
        get_logger(),
        "New cup detected."
    );

    executeCupRoutine();

    robot_busy_ = false;
}

