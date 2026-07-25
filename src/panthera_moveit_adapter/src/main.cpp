#include <memory>

#include <rclcpp/rclcpp.hpp>

#include "panthera_moveit_adapter/moveit_adapter_node.hpp"

int main(
    int argc,
    char * argv[]
)
{
    rclcpp::init(argc, argv);

    auto node =
        std::make_shared<MoveItAdapterNode>();

    rclcpp::spin(node);

    rclcpp::shutdown();

    return 0;
}