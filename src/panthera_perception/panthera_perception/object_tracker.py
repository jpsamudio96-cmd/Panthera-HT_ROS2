#!/usr/bin/env python3

import rclpy

from rclpy.node import Node

from panthera_interfaces.msg import DetectedObject


class ObjectTracker(Node):

    def __init__(self):

        super().__init__("object_tracker")

        self.get_logger().info(
            "Object Tracker iniciado."
        )

        self.subscription = self.create_subscription(

            DetectedObject,

            "/vision/detected_object",

            self.object_callback,

            10

        )

        self.current_object = None

    # Callback
    def object_callback(
        self,
        msg
    ):

        self.get_logger().info(

            "\n"

            "=========================================\n"

            "Object Tracker\n\n"

            f"Class       : {msg.class_name}\n"

            f"Confidence  : {msg.confidence:.2f}\n\n"

            "Image\n"

            f"  u         : {msg.centroid_u}\n"

            f"  v         : {msg.centroid_v}\n\n"

            "Workspace\n"

            f"  X         : {msg.workspace_x:.3f} m\n"

            f"  Y         : {msg.workspace_y:.3f} m\n"

            f"  Z         : {msg.workspace_z:.3f} m\n\n"

            f"Theta       : {msg.theta:.2f}°\n\n"

            "BBox\n"

            f"  x         : {msg.bbox_x}\n"

            f"  y         : {msg.bbox_y}\n"

            f"  w         : {msg.bbox_width}\n"

            f"  h         : {msg.bbox_height}\n"

            "========================================="

        )

        self.current_object = msg

def main(args=None):

    rclpy.init(args=args)

    node = ObjectTracker()

    rclpy.spin(node)

    node.destroy_node()

    rclpy.shutdown()


if __name__ == "__main__":
    main()