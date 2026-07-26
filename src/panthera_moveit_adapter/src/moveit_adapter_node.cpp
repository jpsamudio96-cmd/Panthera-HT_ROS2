#include "panthera_moveit_adapter/moveit_adapter_node.hpp"

MoveItAdapter::MoveItAdapter(
    const rclcpp::Node::SharedPtr& node
)
    : node_(node),
      planning_requested_(false)
{
    RCLCPP_INFO(
        node_->get_logger(),
        "Panthera MoveIt Adapter initialized."
    );

    target_subscription_ =
        node_->create_subscription<
            panthera_interfaces::msg::DetectedObject
        >(
            "/perception/target_object",
            10,
            std::bind(
                &MoveItAdapter::targetCallback,
                this,
                std::placeholders::_1
            )
        );
}

    geometry_msgs::msg::Pose MoveItAdapter::buildTargetPose(
        const panthera_interfaces::msg::DetectedObject& object
    )
    {
        geometry_msgs::msg::Pose pose;

        pose.position.x = object.workspace_x;
        pose.position.y = object.workspace_y;
        pose.position.z = 0.20;

        pose.orientation.x = 0.0;
        pose.orientation.y = 0.0;
        pose.orientation.z = 0.0;
        pose.orientation.w = 1.0;

        return pose;
    }

    bool MoveItAdapter::validateTargetPose(
        const geometry_msgs::msg::Pose& pose
    )
    {
        //
        // Validación temporal del workspace.
        // Los límites exactos se calibrarán más adelante.
        //

        if (pose.position.z < 0.0)
        {
            return false;
        }

        return true;
    }

    bool MoveItAdapter::prepareMotionPlan(
        const geometry_msgs::msg::Pose& target_pose
    )
    {

        RCLCPP_INFO(
            node_->get_logger(),
            "Motion planning stage reached."
        );

        planning_requested_ = true;

        RCLCPP_INFO(
            node_->get_logger(),
            "Planning target:"
        );

        RCLCPP_INFO(
            node_->get_logger(),
            "  Position: [%.4f %.4f %.4f]",
            target_pose.position.x,
            target_pose.position.y,
            target_pose.position.z
        );

        RCLCPP_INFO(
            node_->get_logger(),
            "  Orientation: [%.4f %.4f %.4f %.4f]",
            target_pose.orientation.x,
            target_pose.orientation.y,
            target_pose.orientation.z,
            target_pose.orientation.w
        );

        arm_->setStartStateToCurrentState();

//        arm_->setPoseTarget(target_pose);

        geometry_msgs::msg::Pose pose;

        pose.position.x = 0.20;
        pose.position.y = 0.00;
        pose.position.z = 0.20;

        pose.orientation.w = 1.0;

        RCLCPP_INFO(
            node_->get_logger(),
            "TEST POSE: [%.3f %.3f %.3f]",
            pose.position.x,
            pose.position.y,
            pose.position.z
        );
        
        arm_->setPoseTarget(pose);
        
        RCLCPP_INFO(
            node_->get_logger(),
            "Planning Frame: %s",
            arm_->getPlanningFrame().c_str()
        );

        RCLCPP_INFO(
            node_->get_logger(),
            "End Effector: %s",
            arm_->getEndEffectorLink().c_str()
        );

//        auto current_pose = arm_->getCurrentPose();
//
//        RCLCPP_INFO(
//            node_->get_logger(),
//            "Current EE Pose: [%.3f %.3f %.3f]",
//            current_pose.pose.position.x,
//            current_pose.pose.position.y,
//            current_pose.pose.position.z
//        );

 //       arm_->setNamedTarget("home");
        
        arm_->setPlanningTime(10.0);

        arm_->setNumPlanningAttempts(10);

        arm_->setMaxVelocityScalingFactor(0.2);

        arm_->setMaxAccelerationScalingFactor(0.2);

        auto result = arm_->plan(current_plan_);

        RCLCPP_INFO(
            node_->get_logger(),
            "Planning result code: %d",
            result.val
        );

//       auto result = arm_->plan(current_plan_);
//       auto result = arm_->plan(current_plan_);

        if (
            result != moveit::core::MoveItErrorCode::SUCCESS
        )
        {
            RCLCPP_WARN(
                node_->get_logger(),
                "Motion planning failed."
            );

            return false;
        }

        RCLCPP_INFO(
            node_->get_logger(),
            "Motion plan generated successfully."
        );

        return true;
    }

void MoveItAdapter::initializeMoveGroup()
{
    //
    // Create dedicated ROS2 node for MoveIt
    //

    rclcpp::NodeOptions options;

    options.automatically_declare_parameters_from_overrides(true);

    options.parameter_overrides(
    {
        rclcpp::Parameter(
            "use_sim_time",
            true
        )
    });

    moveit_node_ =
        std::make_shared<rclcpp::Node>(
            "panthera_moveit_client",
            options
        );

    moveit_executor_.add_node(
        moveit_node_
    );

    moveit_spin_thread_ =
        std::thread(
            [this]()
            {
                moveit_executor_.spin();
            }
        );

    arm_ =
        std::make_shared<MoveGroupInterface>(
            moveit_node_,
            "arm"
        );

    RCLCPP_INFO(
        node_->get_logger(),
        "MoveGroupInterface initialized."
    );
}

void MoveItAdapter::targetCallback(
    const panthera_interfaces::msg::DetectedObject::SharedPtr msg
)
{
    //
    // Convertir el objetivo detectado a una Pose de ROS2
    //

    target_pose_ = buildTargetPose(*msg);

    if (!validateTargetPose(target_pose_))
    {
        RCLCPP_WARN(
            node_->get_logger(),
            "Invalid target pose received."
        );

        return;
    }

    if (!prepareMotionPlan(target_pose_))
    {
        RCLCPP_WARN(
            node_->get_logger(),
            "Unable to prepare motion plan."
        );

        return;
    }

    RCLCPP_INFO(
        node_->get_logger(),
        "Target [%s] processed successfully.",
        msg->class_name.c_str()
    );
}