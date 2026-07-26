#!/usr/bin/env python3

import rclpy

from rclpy.node import Node

from panthera_interfaces.msg import DetectedObject


class TargetSelector(Node):

    def __init__(self):

        super().__init__("target_selector")

        self.last_class_name = None

        self.last_detection_time = self.get_clock().now()

        self.timeout = 2.0  # segundos

        self.timer = self.create_timer(
            0.1,
            self.check_detection_timeout
        )

        self.get_logger().info(
            "Target Selector iniciado."
        )

        self.current_target = None

        # Publicador
        self.publisher = self.create_publisher(

            DetectedObject,

            "/perception/target_object",

            10

        )

        # Suscriptor
        self.subscription = self.create_subscription(

            DetectedObject,

            "/vision/detected_object",

            self.object_callback,

            10

        )
    def object_callback(
        self,
        msg
    ):
        self.last_detection_time = self.get_clock().now()

        if msg.class_name == self.last_class_name:
            return

        self.last_class_name = msg.class_name

        self.current_target = msg

        self.publisher.publish(msg)

        self.get_logger().info(
            f"[TARGET] "
            f"{msg.class_name} "
            f"({msg.confidence:.2f})"
        )

    def check_detection_timeout(self):

        now = self.get_clock().now()

        elapsed = (
            now - self.last_detection_time
        ).nanoseconds / 1e9

        if (
            self.last_class_name is not None
            and elapsed > self.timeout
        ):
            self.get_logger().info(
                "No detections. Rearming Target Selector."
            )

            self.last_class_name = None
            self.current_target = None

def main(args=None):

    rclpy.init(args=args)

    node = TargetSelector()

    rclpy.spin(node)

    node.destroy_node()

    rclpy.shutdown()


if __name__ == "__main__":
    main()