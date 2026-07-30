from enum import Enum, auto


class BehaviorState(Enum):

    IDLE = auto()

    OBJECT_READY = auto()

    EXECUTING = auto()

    RETURNING_HOME = auto()


from panthera_behavior.detection_validator import DetectionValidator


class StateMachine:

    def __init__(self, validator: DetectionValidator):

        self._state = BehaviorState.IDLE

        self.validator = validator

    @property
    def state(self):

        return self._state

    def set_state(self, new_state: BehaviorState):

        self._state = new_state

    def transition_to(self, new_state: BehaviorState) -> bool:

        if new_state == self._state:
            return False

        self._state = new_state

        return True

    def tick(self, context) -> bool:

        match self._state:

            case BehaviorState.IDLE:

                if self.validator.is_valid(context):

                    return self.transition_to(
                        BehaviorState.OBJECT_READY
                    )

            case BehaviorState.OBJECT_READY:

                context.processed = True

                return False

            case BehaviorState.EXECUTING:

                return False

            case BehaviorState.RETURNING_HOME:

                context.valid = False
                context.processed = False
                context.detection = None

                return self.transition_to(
                    BehaviorState.IDLE
                )

        return False