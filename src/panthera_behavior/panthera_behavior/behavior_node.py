#!/usr/bin/env python3

import rclpy
import time

from rclpy.node import Node

from panthera_interfaces.msg import DetectedObject

from panthera_behavior.detection_context import DetectionContext
from panthera_behavior.state_machine import StateMachine
from panthera_behavior.detection_validator import DetectionValidator

from panthera_motion.moveit_motion_executor import MoveItMotionExecutor
from panthera_motion.motion_request import MotionRequest, MotionCommand

from panthera_motion.motion_result import MotionStatus
from panthera_behavior.state_machine import BehaviorState

from rclpy.executors import MultiThreadedExecutor

class BehaviorNode(Node):

    def __init__(self):

        super().__init__("panthera_behavior")

        self.detection_context = DetectionContext()

        self.validator = DetectionValidator()

        self.motion_executor = MoveItMotionExecutor(self)

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


    def execute_routine(
        self,
        target_pose: MotionCommand
    ) -> bool:

        self.get_logger().info(
            f"Executing routine -> {target_pose.name}"
        )

        sequence = [

            MotionCommand.GO_HOME,

            MotionCommand.GRIPPER_OPEN,

            target_pose,

            MotionCommand.GRIPPER_CLOSE,

            MotionCommand.GO_HOME,

        ]

        for command in sequence:

            result = self.motion_executor.execute(
                MotionRequest(command)
            )

            self.get_logger().info(
                f"{command.name} -> {result.status.name}"
            )

            if result.status != MotionStatus.SUCCESS:

                self.get_logger().error(
                    f"Routine aborted on {command.name}"
                )

                return False

            # Dar tiempo a que MoveIt termine antes de enviar
            # el siguiente comando
            time.sleep(3.0)

        return True

    def timer_callback(self):

        transition = self.state_machine.tick(
            self.detection_context
        )

        if transition:

            self.get_logger().info(
                f"State -> {self.state_machine.state.name}"
            )

        if self.state_machine.state == BehaviorState.OBJECT_READY:

            detection = self.detection_context.detection

            if detection.color == "red":

                success = self.execute_routine(
                    MotionCommand.GO_POSE1
                )

            elif detection.color == "green":

                success = self.execute_routine(
                    MotionCommand.GO_POSE2
                )

            else:

                self.get_logger().warning(
                    f"Unsupported color: {detection.color}"
                )

                return

            if success:

                self.state_machine.set_state(
                    BehaviorState.RETURNING_HOME
                )

                self.get_logger().info(
                    f"State -> {self.state_machine.state.name}"
                )


def main():

    rclpy.init()

    node = BehaviorNode()

    executor = MultiThreadedExecutor()

    executor.add_node(node)

    try:

        executor.spin()

    finally:

        executor.shutdown()

        node.destroy_node()

        rclpy.shutdown()


if __name__ == "__main__":

    main()