#!/usr/bin/env python3

import cv2

from cv_bridge import CvBridge


class ImageEncoder:

    def __init__(self):

        self.bridge = CvBridge()

    def ros_to_cv(self, image_msg):

        return self.bridge.imgmsg_to_cv2(
            image_msg,
            desired_encoding="rgb8",
        )

    def encode_jpeg(self, cv_image, quality=80):

        """
        Convert OpenCV image -> JPEG bytes
        """

        success, buffer = cv2.imencode(
            ".jpg",
            cv_image,
            [
                cv2.IMWRITE_JPEG_QUALITY,
                quality,
            ],
        )

        if not success:
            raise RuntimeError("JPEG encoding failed")

        return buffer.tobytes()

    def ros_to_jpeg(self, image_msg, quality=80):
        cv_image = self.ros_to_cv(image_msg)
        return self.encode_jpeg(cv_image, quality)

