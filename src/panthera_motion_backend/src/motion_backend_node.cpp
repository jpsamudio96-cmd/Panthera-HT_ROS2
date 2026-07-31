#include "panthera_motion_backend/motion_backend_node.hpp"
#include <moveit/planning_interface/planning_interface.h>
#include <moveit/move_group_interface/move_group_interface.h>

#include <thread>

MotionBackendNode::MotionBackendNode()
    : Node("panthera_motion_backend")
{
    action_server_ =
        rclcpp_action::create_server<ExecuteMotion>(
            this,
            "execute_motion",

            std::bind(
                &MotionBackendNode::handleGoal,
                this,
                std::placeholders::_1,
                std::placeholders::_2
            ),

            std::bind(
                &MotionBackendNode::handleCancel,
                this,
                std::placeholders::_1
            ),

            std::bind(
                &MotionBackendNode::handleAccepted,
                this,
                std::placeholders::_1
            )
        );

    RCLCPP_INFO(
        get_logger(),
        "Motion Backend started."
    );
}

void MotionBackendNode::initializeMoveIt()
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

bool MotionBackendNode::executeNamedTarget(
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
            "Planning failed for '%s'.",
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
            "Execution failed for '%s'.",
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

rclcpp_action::GoalResponse
MotionBackendNode::handleGoal(
    const rclcpp_action::GoalUUID &,
    std::shared_ptr<const ExecuteMotion::Goal> goal
)
{
    RCLCPP_INFO(
        get_logger(),
        "Received command: %s",
        goal->command.c_str()
    );

    return rclcpp_action::GoalResponse::ACCEPT_AND_EXECUTE;
}

rclcpp_action::CancelResponse
MotionBackendNode::handleCancel(
    const std::shared_ptr<GoalHandle>
)
{
    RCLCPP_INFO(
        get_logger(),
        "Cancel requested."
    );

    return rclcpp_action::CancelResponse::ACCEPT;
}

void MotionBackendNode::handleAccepted(
    const std::shared_ptr<GoalHandle> goal_handle
)
{
    std::thread(
        [this, goal_handle]()
        {
            execute(goal_handle);
        }
    ).detach();
}

void MotionBackendNode::execute(
    const std::shared_ptr<GoalHandle> goal_handle
)
{
    if (!arm_)
    {
        initializeMoveIt();
    }

    const auto goal = goal_handle->get_goal();

    auto feedback =
        std::make_shared<ExecuteMotion::Feedback>();

    feedback->current_state = "Executing";

    goal_handle->publish_feedback(feedback);

    bool success = false;

    if (goal->command == "GO_HOME")
    {
        success = executeNamedTarget("home");
    }
    else if (goal->command == "EXECUTE_APPROACH")
    {
        success = executeNamedTarget("pose1");
    }
    else
    {
        RCLCPP_ERROR(
            get_logger(),
            "Unknown command: %s",
            goal->command.c_str()
        );
    }

    auto result =
        std::make_shared<ExecuteMotion::Result>();

    result->success = success;

    if (success)
    {
        result->message =
            "Motion executed successfully.";

        goal_handle->succeed(result);
    }
    else
    {
        result->message =
            "Motion execution failed.";

        goal_handle->abort(result);
    }
}