#!/usr/bin/env python3

import struct


class PCBP:

    HEADER_FORMAT = "!I"      # Unsigned Int (4 bytes, network byte order)
    HEADER_SIZE = 4

    def build_packet(self, payload: bytes) -> bytes:
        """
        Build a PCBP packet:
        +----------------+
        | uint32 length  |
        +----------------+
        | payload        |
        +----------------+
        """

        header = struct.pack(
            self.HEADER_FORMAT,
            len(payload),
        )

        return header + payload