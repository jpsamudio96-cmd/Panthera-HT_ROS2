#pragma once

#include <moveit/move_group_interface/move_group_interface.h>

#include <memory>

#include <rclcpp/rclcpp.hpp>
#include <rclcpp_action/rclcpp_action.hpp>

#include "panthera_interfaces/action/execute_motion.hpp"

class MotionBackendNode : public rclcpp::Node
{
public:
    using MoveGroupInterface =
    moveit::planning_interface::MoveGroupInterface;

    using ExecuteMotion =
        panthera_interfaces::action::ExecuteMotion;

    using GoalHandle =
        rclcpp_action::ServerGoalHandle<ExecuteMotion>;

    MotionBackendNode();

private:

    void initializeMoveIt();

    bool executeNamedTarget(
        const std::string& target_name
    );

    bool executeGripperTarget(
        const std::string& target_name
    );

    std::shared_ptr<MoveGroupInterface> arm_;
    std::shared_ptr<MoveGroupInterface> gripper_;

    rclcpp_action::Server<ExecuteMotion>::SharedPtr action_server_;

    rclcpp_action::GoalResponse handleGoal(
        const rclcpp_action::GoalUUID & uuid,
        std::shared_ptr<const ExecuteMotion::Goal> goal
    );

    rclcpp_action::CancelResponse handleCancel(
        const std::shared_ptr<GoalHandle> goal_handle
    );

    void handleAccepted(
        const std::shared_ptr<GoalHandle> goal_handle
    );

    void execute(
        const std::shared_ptr<GoalHandle> goal_handle
    );


};