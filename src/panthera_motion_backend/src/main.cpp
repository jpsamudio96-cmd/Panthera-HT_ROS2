#include "panthera_motion_backend/motion_backend_node.hpp"

int main(int argc, char ** argv)
{
    rclcpp::init(argc, argv);

    auto node =
        std::make_shared<MotionBackendNode>();

    rclcpp::spin(node);

    rclcpp::shutdown();

    return 0;
}