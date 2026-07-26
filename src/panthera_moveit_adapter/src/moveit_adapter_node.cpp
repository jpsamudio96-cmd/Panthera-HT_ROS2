#include "panthera_moveit_adapter/moveit_adapter_node.hpp"

#include <chrono>
#include <functional>

using namespace std::chrono_literals;

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

    gripper_ = std::make_shared<MoveGroupInterface>(
        shared_from_this(),
        "gripper"
    );

    arm_->setMaxVelocityScalingFactor(0.30);
    arm_->setMaxAccelerationScalingFactor(0.30);

    gripper_->setMaxVelocityScalingFactor(1.0);
    gripper_->setMaxAccelerationScalingFactor(1.0);

    RCLCPP_INFO(
        get_logger(),
        "MoveGroupInterface initialized."
    );
}

bool MoveItAdapterNode::executeNamedTarget(
    const std::string& target_name
)
{
    if (!arm_)
    {
        RCLCPP_ERROR(
            get_logger(),
            "MoveGroupInterface not initialized."
        );
        return false;
    }

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

bool MoveItAdapterNode::executeGripperTarget(
    const std::string& target_name
)
{
    if (!gripper_)
    {
        RCLCPP_ERROR(
            get_logger(),
            "Gripper MoveGroupInterface not initialized."
        );
        return false;
    }

    gripper_->setStartStateToCurrentState();

    gripper_->setNamedTarget(target_name);

    MoveGroupInterface::Plan plan;

    auto result = gripper_->plan(plan);

    if (result != moveit::core::MoveItErrorCode::SUCCESS)
    {
        RCLCPP_ERROR(
            get_logger(),
            "Gripper planning failed for '%s'.",
            target_name.c_str()
        );
        return false;
    }

    result = gripper_->execute(plan);

    if (result != moveit::core::MoveItErrorCode::SUCCESS)
    {
        RCLCPP_ERROR(
            get_logger(),
            "Gripper execution failed for '%s'.",
            target_name.c_str()
        );
        return false;
    }

    return true;
}

void MoveItAdapterNode::executeCupRoutine()
{
    if (!executeNamedTarget("pose1"))
    {
        robot_busy_ = false;
        return;
    }

    rclcpp::sleep_for(1s);

    executeGripperTarget("open");

    executeNamedTarget("home");

    robot_busy_ = false;
}

void MoveItAdapterNode::executeBottleRoutine()
{
    if (!executeNamedTarget("pose1"))
    {
        robot_busy_ = false;
        return;
    }

    executeGripperTarget("close");

    if (!executeNamedTarget("pose2"))
    {
        robot_busy_ = false;
        return;
    }

    executeGripperTarget("open");

    executeNamedTarget("home");

    robot_busy_ = false;
}

bool MoveItAdapterNode::dispatchObject(
    const std::string& object_name
)
{
    if (object_name == "cup")
    {
        executeCupRoutine();
        return true;
    }

    if (object_name == "bottle")
    {
        executeBottleRoutine();
        return true;
    }

    return false;
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

    const std::string object_name = msg->class_name;

    if (
        object_name != "cup" &&
        object_name != "bottle"
    )
    {
        return;
    }

    robot_busy_ = true;

    dispatchObject(object_name);
}