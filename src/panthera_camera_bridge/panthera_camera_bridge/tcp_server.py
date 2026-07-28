#!/usr/bin/env python3

import socket

import threading


class TCPServer:

    def __init__(self, host="0.0.0.0", port=5002):

        self.host = host
        self.port = port

        self.server_socket = None
        self.client_socket = None
        self.client_address = None

    def start(self):

        self.server_socket = socket.socket(
            socket.AF_INET,
            socket.SOCK_STREAM,
        )

        self.server_socket.setsockopt(
            socket.SOL_SOCKET,
            socket.SO_REUSEADDR,
            1,
        )

        self.server_socket.bind(
            (self.host, self.port)
        )

        self.server_socket.listen(1)

        print(f"TCP Server listening on {self.host}:{self.port}")

    def wait_for_client(self):

        self.client_socket, self.client_address = (
            self.server_socket.accept()
        )

        print(
            f"Client connected: {self.client_address}"
        )

    def close(self):

        if self.client_socket is not None:
            self.client_socket.close()

        if self.server_socket is not None:
            self.server_socket.close()

    def start_accept_thread(self):

        thread = threading.Thread(
            target=self.wait_for_client,
            daemon=True,
        )

        thread.start()

    def send(self, data):

        if self.client_socket is None:
            return False

        try:

            self.client_socket.sendall(data)

            return True

        except Exception:

            self.client_socket.close()

            self.client_socket = None

            self.client_address = None

            return False