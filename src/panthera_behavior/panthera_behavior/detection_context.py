from dataclasses import dataclass
from typing import Optional

from panthera_interfaces.msg import DetectedObject


@dataclass
class DetectionContext:

    detection: Optional[DetectedObject] = None

    valid: bool = False

    processed: bool = False

    locked: bool = False
    