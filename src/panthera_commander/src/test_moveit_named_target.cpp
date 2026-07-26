#include <chrono>
#include <thread>

#include <rclcpp/rclcpp.hpp>
#include <rclcpp/executors/single_threaded_executor.hpp>

#include <moveit/move_group_interface/move_group_interface.h>

using MoveGroupInterface = moveit::planning_interface::MoveGroupInterface;

int main(int argc, char ** argv)
{
    rclcpp::init(argc, argv);

    rclcpp::NodeOptions node_options;
    node_options.automatically_declare_parameters_from_overrides(true);

    auto node =
        std::make_shared<rclcpp::Node>(
            "test_moveit_named_target",
            node_options
        );

    rclcpp::executors::SingleThreadedExecutor executor;
    executor.add_node(node);

    std::thread executor_thread(
        [&executor]()
        {
            executor.spin();
        });

    RCLCPP_INFO(
        node->get_logger(),
        "Waiting for MoveIt..."
    );

    rclcpp::sleep_for(std::chrono::seconds(3));

    MoveGroupInterface arm(node, "arm");

    arm.setMaxVelocityScalingFactor(0.3);
    arm.setMaxAccelerationScalingFactor(0.3);

    arm.setStartStateToCurrentState();

    RCLCPP_INFO(
        node->get_logger(),
        "Planning to named target: pose1"
    );

    arm.setNamedTarget("pose1");

    MoveGroupInterface::Plan plan;

    auto result = arm.plan(plan);

    if (result == moveit::core::MoveItErrorCode::SUCCESS)
    {
        RCLCPP_INFO(
            node->get_logger(),
            "Planning succeeded."
        );

        result = arm.execute(plan);

        if (result == moveit::core::MoveItErrorCode::SUCCESS)
        {
            RCLCPP_INFO(
                node->get_logger(),
                "Execution completed."
            );
        }
        else
        {
            RCLCPP_ERROR(
                node->get_logger(),
                "Execution failed."
            );
        }
    }
    else
    {
        RCLCPP_ERROR(
            node->get_logger(),
            "Planning failed."
        );
    }

    executor.cancel();
    executor_thread.join();

    rclcpp::shutdown();

    return 0;
}