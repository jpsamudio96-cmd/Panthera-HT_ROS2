#include <memory>
#include <thread>
#include <functional>

#include "rclcpp/rclcpp.hpp"

#include "geometry_msgs/msg/pose.hpp"

#include "moveit/move_group_interface/move_group_interface.h"
#include "moveit/robot_state/robot_state.h"

#include "panthera_interfaces/msg/detected_object.hpp"

using MoveGroupInterface =
    moveit::planning_interface::MoveGroupInterface;

class TestMoveItPose
{
public:

    explicit TestMoveItPose(
        const rclcpp::Node::SharedPtr& node
    )
        : node_(node)
    {
        arm_ =
            std::make_shared<MoveGroupInterface>(
                node_,
                "arm"
            );

        subscription_ =
            node_->create_subscription<
                panthera_interfaces::msg::DetectedObject
            >(
                "/perception/target_object",
                10,
                std::bind(
                    &TestMoveItPose::callback,
                    this,
                    std::placeholders::_1
                )
            );

        RCLCPP_INFO(
            node_->get_logger(),
            "Waiting for detections..."
        );
    }

private:

    void callback(
        const panthera_interfaces::msg::DetectedObject::SharedPtr /*msg*/
    )
    {
        RCLCPP_INFO(
            node_->get_logger(),
            "=========================================="
        );

        RCLCPP_INFO(
            node_->get_logger(),
            "Detection received."
        );

        RCLCPP_INFO(
            node_->get_logger(),
            "Planning frame: %s",
            arm_->getPlanningFrame().c_str()
        );

        RCLCPP_INFO(
            node_->get_logger(),
            "End effector: %s",
            arm_->getEndEffectorLink().c_str()
        );

        geometry_msgs::msg::PoseStamped current_pose =
            arm_->getCurrentPose();

        RCLCPP_INFO(
            node_->get_logger(),
            "Current Pose:"
        );

        RCLCPP_INFO(
            node_->get_logger(),
            "  Position: [%.3f, %.3f, %.3f]",
            current_pose.pose.position.x,
            current_pose.pose.position.y,
            current_pose.pose.position.z
        );

        RCLCPP_INFO(
            node_->get_logger(),
            "  Orientation: [%.3f, %.3f, %.3f, %.3f]",
            current_pose.pose.orientation.x,
            current_pose.pose.orientation.y,
            current_pose.pose.orientation.z,
            current_pose.pose.orientation.w
        );

        geometry_msgs::msg::Pose pose;

        pose.position.x = 0.20;
        pose.position.y = 0.00;
        pose.position.z = 0.20;

        pose.orientation.x = 0.0;
        pose.orientation.y = 0.0;
        pose.orientation.z = 0.0;
        pose.orientation.w = 1.0;

        RCLCPP_INFO(
            node_->get_logger(),
            "Target Pose:"
        );

        RCLCPP_INFO(
            node_->get_logger(),
            "  Position: [%.3f, %.3f, %.3f]",
            pose.position.x,
            pose.position.y,
            pose.position.z
        );

        RCLCPP_INFO(
            node_->get_logger(),
            "  Orientation: [%.3f, %.3f, %.3f, %.3f]",
            pose.orientation.x,
            pose.orientation.y,
            pose.orientation.z,
            pose.orientation.w
        );

        arm_->setStartStateToCurrentState();

        auto state =
            arm_->getCurrentState(
                2.0
            );

        if (!state)
        {
            RCLCPP_ERROR(
                node_->get_logger(),
                "Unable to obtain current RobotState."
            );

            return;
        }

        const moveit::core::JointModelGroup* group =
            arm_->getRobotModel()->getJointModelGroup(
                "arm"
            );

        if (!group)
        {
            RCLCPP_ERROR(
                node_->get_logger(),
                "JointModelGroup 'arm' not found."
            );

            return;
        }

        RCLCPP_INFO(
            node_->get_logger(),
            "JointModelGroup found successfully."
        );

        bool ik_ok =
            state->setFromIK(
                group,
                pose,
                2.0
            );

        RCLCPP_INFO(
            node_->get_logger(),
            "IK RESULT = %s",
            ik_ok ? "SUCCESS" : "FAILED"
        );

        RCLCPP_INFO(
            node_->get_logger(),
            "=========================================="
        );
    }

    rclcpp::Node::SharedPtr node_;

    std::shared_ptr<MoveGroupInterface> arm_;

    rclcpp::Subscription<
        panthera_interfaces::msg::DetectedObject
    >::SharedPtr subscription_;
};

int main(
    int argc,
    char** argv
)
{
    rclcpp::init(
        argc,
        argv
    );

    auto node =
        std::make_shared<rclcpp::Node>(
            "test_moveit_pose",
            rclcpp::NodeOptions()
                .automatically_declare_parameters_from_overrides(true)
        );

    node->set_parameter(
        rclcpp::Parameter(
            "use_sim_time",
            true
        )
    );

    rclcpp::executors::SingleThreadedExecutor executor;

    executor.add_node(
        node
    );

    std::thread spin_thread(
        [&executor]()
        {
            executor.spin();
        }
    );

    TestMoveItPose tester(
        node
    );

    spin_thread.join();

    rclcpp::shutdown();

    return 0;
}