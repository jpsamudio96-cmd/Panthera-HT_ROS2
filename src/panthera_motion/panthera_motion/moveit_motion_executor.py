import rclpy
from rclpy.action import ActionClient

from panthera_interfaces.action import ExecuteMotion

from panthera_motion.imotion_executor import IMotionExecutor
from panthera_motion.motion_request import MotionRequest, MotionCommand
from panthera_motion.motion_result import MotionResult, MotionStatus


class MoveItMotionExecutor(IMotionExecutor):

    _COMMAND_MAP = {

        MotionCommand.GO_HOME: "GO_HOME",

        MotionCommand.GO_POSE1: "GO_POSE1",

        MotionCommand.GO_POSE2: "GO_POSE2",

        MotionCommand.GRIPPER_OPEN: "GRIPPER_OPEN",

        MotionCommand.GRIPPER_CLOSE: "GRIPPER_CLOSE",
    }

    def __init__(self, node):

        self._node = node

        self._busy = False

        self._action_client = ActionClient(
            self._node,
            ExecuteMotion,
            "execute_motion"
        )

    def execute(
        self,
        request: MotionRequest
    ) -> MotionResult:

        self._busy = True

        self._action_client.wait_for_server()

        goal = ExecuteMotion.Goal()
        goal.command = self._COMMAND_MAP[request.command]

        self._action_client.send_goal_async(goal)

        self._busy = False

        return MotionResult(MotionStatus.SUCCESS)

    def is_busy(self) -> bool:

        return self._busy