from dataclasses import dataclass
from enum import Enum, auto


class MotionStatus(Enum):
    SUCCESS = auto()
    FAILURE = auto()
    RUNNING = auto()


@dataclass
class MotionResult:
    status: MotionStatus