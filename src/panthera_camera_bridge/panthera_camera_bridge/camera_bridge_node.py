#!/usr/bin/env python3

import rclpy
from rclpy.node import Node

from sensor_msgs.msg import Image

from panthera_camera_bridge.image_encoder import ImageEncoder

from panthera_camera_bridge.tcp_server import TCPServer
from panthera_camera_bridge.pcbp import PCBP

class CameraBridgeNode(Node):

    def __init__(self):
        super().__init__("camera_bridge_node")

        self.frame_count = 0

        self.encoder = ImageEncoder()

        self.get_logger().info("Panthera Camera Bridge initialized")

        self.server = TCPServer(port=5002)

        self.server.start()
        self.server.start_accept_thread()

        self.protocol = PCBP()

        self.subscription = self.create_subscription(
            Image,
            "/industrial_camera/image_raw",
            self.image_callback,
            10,
        )

    def image_callback(self, msg):

        # Convert ROS Image -> OpenCV
        image = self.encoder.ros_to_cv(msg)

        # Encode OpenCV -> JPEG
        jpeg = self.encoder.encode_jpeg(image)

        self.frame_count += 1

        # Log cada 30 imágenes
        if self.frame_count % 30 == 0:

            self.get_logger().info(
                f"Frame {self.frame_count}"
            )

            self.get_logger().info(
                f"Image shape : {image.shape}"
            )

            self.get_logger().info(
                f"JPEG size   : {len(jpeg)} bytes"
            )

            # Solo intentar enviar si existe un cliente conectado
            if self.server.client_socket is not None:

                payload = jpeg

                packet = self.protocol.build_packet(payload)

                if self.server.send(packet):

                    self.get_logger().info(
                        "Test packet sent"
                    )

                else:

                    self.get_logger().warn(
                        "TCP client disconnected"
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