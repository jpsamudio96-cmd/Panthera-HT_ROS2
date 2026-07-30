#!/usr/bin/env python3

import rclpy

from rclpy.node import Node

from panthera_interfaces.msg import DetectedObject

from panthera_behavior.detection_context import DetectionContext
from panthera_behavior.state_machine import StateMachine
from panthera_behavior.detection_validator import DetectionValidator

from panthera_motion.mock_motion_executor import MockMotionExecutor
from panthera_motion.motion_request import MotionRequest, MotionCommand

from panthera_motion.motion_result import MotionStatus
from panthera_behavior.state_machine import BehaviorState

class BehaviorNode(Node):

    def __init__(self):

        super().__init__("panthera_behavior")

        self.detection_context = DetectionContext()

        self.validator = DetectionValidator()

        self.motion_executor = MockMotionExecutor()

        self.state_machine = StateMachine(
            self.validator
        )

        self.get_logger().info(
            "Panthera Behavior Node started."
        )

        #self.last_detection = None

        # Vision subscriber
        self.subscription = self.create_subscription(
            DetectedObject,
            "/vision/detected_object",
            self.detection_callback,
            10
        )

        self.timer = self.create_timer(

            0.1,

            self.timer_callback

        )


    def detection_callback(self, msg: DetectedObject):

        self.detection_context.detection = msg
        self.detection_context.valid = True
        self.detection_context.processed = False

        self.get_logger().info(

            "\n"
            "=========================================\n"
            "         DETECTION RECEIVED\n"
            "=========================================\n"
            f"YOLO Class      : {msg.class_name}\n"
            f"Panthera Class : {msg.panthera_class}\n"
            f"Shape          : {msg.shape}\n"
            f"Color          : {msg.color}\n"
            f"Confidence     : {msg.confidence:.2f}\n"
            "\n"
            f"Centroid (px)  : ({msg.centroid_u}, {msg.centroid_v})\n"
            f"Theta          : {msg.theta:.2f} deg\n"
            "\n"
            f"Workspace X    : {msg.workspace_x:.3f}\n"
            f"Workspace Y    : {msg.workspace_y:.3f}\n"
            f"Workspace Z    : {msg.workspace_z:.3f}\n"
            "\n"
            f"BBox           : "
            f"({msg.bbox_x}, {msg.bbox_y}) "
            f"{msg.bbox_width}x{msg.bbox_height}\n"
            "========================================="
        )

    def execute_motion(
        self,
        command: MotionCommand,
        next_state: BehaviorState
    ):

        request = MotionRequest(command)

        result = self.motion_executor.execute(request)

        self.get_logger().info(
            f"Motion -> {result.status.name}"
        )

        if result.status == MotionStatus.SUCCESS:

            self.state_machine.set_state(next_state)

            self.get_logger().info(
                f"State -> {self.state_machine.state.name}"
            )

    def timer_callback(self):

        transition = self.state_machine.tick(
            self.detection_context
        )

        if transition:

            self.get_logger().info(
                f"State -> {self.state_machine.state.name}"
            )

        if self.state_machine.state == BehaviorState.OBJECT_READY:

            self.execute_motion(
                MotionCommand.EXECUTE_APPROACH,
                BehaviorState.EXECUTING
            )

        elif self.state_machine.state == BehaviorState.EXECUTING:

            self.execute_motion(
                MotionCommand.GO_HOME,
                BehaviorState.RETURNING_HOME
            )


def main():

    rclpy.init()

    node = BehaviorNode()

    rclpy.spin(node)

    node.destroy_node()

    rclpy.shutdown()


if __name__ == "__main__":

    main()