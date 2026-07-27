#!/usr/bin/env python3

import rclpy
from rclpy.node import Node


class CameraBridgeNode(Node):

    def __init__(self):
        super().__init__("camera_bridge_node")

        self.get_logger().info("Panthera Camera Bridge initialized")


def main(args=None):

    rclpy.init(args=args)

    node = CameraBridgeNode()

    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass

    node.destroy_node()
    rclpy.shutdown()


if __name__ == "__main__":
    main()