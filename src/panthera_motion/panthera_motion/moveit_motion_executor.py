import rclpy
from rclpy.action import ActionClient

from panthera_interfaces.action import ExecuteMotion

from panthera_motion.imotion_executor import IMotionExecutor
from panthera_motion.motion_request import MotionRequest, MotionCommand
from panthera_motion.motion_result import MotionResult, MotionStatus


class MoveItMotionExecutor(IMotionExecutor):

    _COMMAND_MAP = {
        MotionCommand.GO_HOME: "GO_HOME",
        MotionCommand.EXECUTE_APPROACH: "EXECUTE_APPROACH",
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

        # Esperar a que el Action Server esté disponible
        self._action_client.wait_for_server()

        # Crear Goal
        goal = ExecuteMotion.Goal()
        goal.command = self._COMMAND_MAP[request.command]

        # Enviar Goal
        goal_future = self._action_client.send_goal_async(goal)

        # Esperar respuesta del servidor
        rclpy.spin_until_future_complete(
            self._node,
            goal_future
        )

        goal_handle = goal_future.result()

        if not goal_handle.accepted:

            self._busy = False

            return MotionResult(MotionStatus.FAILURE)

        # Esperar resultado
        result_future = goal_handle.get_result_async()

        rclpy.spin_until_future_complete(
            self._node,
            result_future
        )

        result = result_future.result().result

        self._busy = False

        if result.success:
            return MotionResult(MotionStatus.SUCCESS)

        return MotionResult(MotionStatus.FAILURE)

    def is_busy(self) -> bool:

        return self._busy