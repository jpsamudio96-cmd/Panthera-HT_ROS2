#!/usr/bin/env python3

import rclpy

from rclpy.node import Node

from panthera_interfaces.msg import DetectedObject


class TargetSelector(Node):

    def __init__(self):

        super().__init__("target_selector")

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

        # Guardar el objetivo actual
        self.current_target = msg

        # Publicar el objetivo seleccionado
        self.publisher.publish(msg)

        self.get_logger().info(

            f"[TARGET] "

            f"{msg.class_name} "

            f"({msg.confidence:.2f})"

        )

def main(args=None):

    rclpy.init(args=args)

    node = TargetSelector()

    rclpy.spin(node)

    node.destroy_node()

    rclpy.shutdown()


if __name__ == "__main__":
    main()