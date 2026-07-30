from panthera_motion.imotion_executor import IMotionExecutor
from panthera_motion.motion_request import MotionRequest
from panthera_motion.motion_result import MotionResult, MotionStatus


class MockMotionExecutor(IMotionExecutor):

    def __init__(self):
        self._busy = False

    def execute(self, request: MotionRequest) -> MotionResult:
        self._busy = True

        # El request aún no se utiliza.
        _ = request

        self._busy = False

        return MotionResult(MotionStatus.SUCCESS)

    def is_busy(self) -> bool:
        return self._busy