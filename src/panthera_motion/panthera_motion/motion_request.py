from dataclasses import dataclass
from enum import Enum, auto


class MotionCommand(Enum):
    GO_HOME = auto()
    EXECUTE_APPROACH = auto()


@dataclass
class MotionRequest:
    command: MotionCommand