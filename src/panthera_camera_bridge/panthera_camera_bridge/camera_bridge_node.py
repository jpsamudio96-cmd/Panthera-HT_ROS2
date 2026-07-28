#!/usr/bin/env python3

import rclpy
from rclpy.node import Node

from sensor_msgs.msg import Image


class CameraBridgeNode(Node):

    def __init__(self):
        super().__init__("camera_bridge_node")

        self.frame_count = 0

        self.get_logger().info("Panthera Camera Bridge initialized")

        self.subscription = self.create_subscription(
            Image,
            "/camera/image_raw",
            self.image_callback,
            10,
        )

    def image_callback(self, msg):
        self.frame_count += 1

        self.get_logger().info(
            f"Frame {self.frame_count}: {msg.width}x{msg.height}"
        )


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