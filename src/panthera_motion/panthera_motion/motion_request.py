from enum import Enum
from dataclasses import dataclass


class MotionCommand(Enum):

    GO_HOME = 1

    GO_POSE1 = 2

    GO_POSE2 = 3

    GRIPPER_OPEN = 4

    GRIPPER_CLOSE = 5


@dataclass
class MotionRequest:

    command: MotionCommand