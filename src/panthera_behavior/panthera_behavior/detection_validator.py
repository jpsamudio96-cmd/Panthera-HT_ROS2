from panthera_behavior.detection_context import DetectionContext


class DetectionValidator:

    MIN_CONFIDENCE = 0.60

    def is_valid(self, context: DetectionContext) -> bool:

        if not context.valid:
            return False

        if context.processed:
            return False

        if context.detection is None:
            return False

        if context.detection.confidence < self.MIN_CONFIDENCE:
            return False

        return True
