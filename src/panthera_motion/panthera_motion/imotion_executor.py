from abc import ABC, abstractmethod

from panthera_motion.motion_request import MotionRequest
from panthera_motion.motion_result import MotionResult


class IMotionExecutor(ABC):

    @abstractmethod
    def execute(self, request: MotionRequest) -> MotionResult:
        pass

    @abstractmethod
    def is_busy(self) -> bool:
        pass