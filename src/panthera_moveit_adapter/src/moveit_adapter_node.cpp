#include "panthera_moveit_adapter/moveit_adapter_node.hpp"

MoveItAdapterNode::MoveItAdapterNode()
    : Node("panthera_moveit_adapter")
{
    RCLCPP_INFO(
        this->get_logger(),
        "Panthera MoveIt Adapter initialized."
    );
}