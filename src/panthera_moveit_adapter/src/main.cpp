#include <memory>
#include <thread>

#include <rclcpp/rclcpp.hpp>

#include "panthera_moveit_adapter/moveit_adapter_node.hpp"

int main(
    int argc,
    char * argv[]
)
{
    rclcpp::init(argc, argv);

    auto node =
        rclcpp::Node::make_shared(
            "panthera_moveit_adapter"
        );
        
    node->set_parameter(
        rclcpp::Parameter(
            "use_sim_time",
            true
        )
        );

    rclcpp::executors::SingleThreadedExecutor executor;

    executor.add_node(node);

    std::thread spinner(
        [&executor]()
        {
            executor.spin();
        }
    );

    auto adapter =
        std::make_shared<MoveItAdapter>(node);

    adapter->initializeMoveGroup();

    spinner.join();

    rclcpp::shutdown();

    return 0;
}