import json
import socket
import threading

import rclpy
from rclpy.node import Node

from panthera_interfaces.msg import DetectedObject


class VisionBridge(Node):
    
    # Constructor
    def __init__(self):

        super().__init__("vision_bridge") 

        #Parámetros ROS2
        self.declare_parameter(
            "host",
            "127.0.0.1"
        )

        self.declare_parameter(
            "port",
            5001
        )
        self.host = self.get_parameter(
            "host"
        ).value

        self.port = self.get_parameter(
            "port"
        ).value

        #Variables internas
        self.client_file = None
        self.server = None
        self.client = None
        self.server_thread = None

        #Publisher ROS2
        self.publisher = self.create_publisher(
            DetectedObject,

            "/vision/detected_object",

            10
        )
        
        #Arrancar el servidor TCP
        self.setup_server() 
        self.get_logger().info("Vision Bridge iniciado")

 # TCP SERVER
    # Configuración del servidor TCP
    def setup_server(self):

        self.server = socket.socket(
            socket.AF_INET,
            socket.SOCK_STREAM
        )

        self.server.setsockopt(
            socket.SOL_SOCKET,
            socket.SO_REUSEADDR,
            1
        )

        self.server.bind((self.host, self.port))

        self.server.listen(1)

        self.server_thread = threading.Thread(
            target=self.wait_for_connection,
            daemon=True
        )

        self.server_thread.start()

        self.get_logger().info(
            f"Servidor TCP escuchando en {self.host}:{self.port}"
        )

    # Esperar conexión del cliente TCP
    def wait_for_connection(self):

        while rclpy.ok():

            self.get_logger().info(
                "Esperando cliente TCP..."
            )

            client, address = self.server.accept()

            self.client = client

            self.get_logger().info(
                f"Cliente conectado desde {address[0]}:{address[1]}"
            )

            self.handle_client()

    # Manejo del cliente TCP
    def handle_client(self):
        # Lee los datos del cliente y procesa los paquetes JSON
        if self.client is None:
            return

        client_file = self.client.makefile(
            "r",
            encoding="utf-8"
        )

        try:

            for line in client_file:

                if not rclpy.ok():
                    break

                text = line.strip()

                if not text:
                    continue

                self.process_packet(text)

        except Exception as e:

            self.get_logger().error(
                f"Error de comunicación TCP: {e}"
            )

        finally:

            self.get_logger().info(
                "Cliente desconectado"
            )

            client_file.close()

            if self.client is not None:

                self.client.close()
                self.client = None

#Procesamiento de paquetes

    # Procesar el paquete recibido
    def process_packet(self, text):

        packet = self.parse_packet(text)

        if packet is None:
            return

        if not self.validate_packet(packet):

            self.get_logger().warning(
                "Paquete TCP inválido"
            )

            return

        for detection in packet["detections"]:

            if not self.validate_detection(detection):

                self.get_logger().warning(
                    "Detección inválida"
                )

                continue

            ros_msg = self.build_detected_object(
                detection
            )

            self.publish_detected_object(
                ros_msg
            )

    # Parsear el paquete JSON
    def parse_packet(self, text):

        try:

            return json.loads(text)

        except json.JSONDecodeError as e:

            self.get_logger().error(
                f"JSON inválido: {e}"
            )

            return None

    # Validar el paquete recibido
    def validate_packet(self, packet):

        required_fields = [

            "protocol_version",

            "timestamp",

            "source",

            "detections"

        ]

        for field in required_fields:

            if field not in packet:

                self.get_logger().warning(
                    f"Falta el campo '{field}'"
                )

                return False

        if packet["protocol_version"] != 1:

            self.get_logger().warning(
                "Versión de protocolo no soportada"
            )

            return False

        if not isinstance(
            packet["detections"],
            list
        ):

            self.get_logger().warning(
                "'detections' debe ser una lista"
            )

            return False

        return True

    # Validar la detección individual
    def validate_detection(self, detection):

        required_fields = [

            "class_name",

            "confidence",

            "x",

            "y",

            "z",

            "frame_id"

        ]

        for field in required_fields:

            if field not in detection:

                self.get_logger().warning(
                    f"Detección incompleta: falta '{field}'"
                )

                return False

        return True

 # JSON
    # Parsear el paquete JSON
    def parse_packet(self, message):

        return json.loads(message)

    # Imprime paquete recibido
    def print_packet(self, packet):

        self.get_logger().info(
            "========== TCP =========="
        )

        self.get_logger().info(
            f"Protocolo : {packet['protocol_version']}"
        )

        self.get_logger().info(
            f"Origen    : {packet['source']}"
        )

        self.get_logger().info(
            f"Timestamp : {packet['timestamp']}"
        )

        for detection in packet["detections"]:

            self.get_logger().info(
                "------ Detección ------"
            )

            self.get_logger().info(
                f"Clase      : {detection['class_name']}"
            )

            self.get_logger().info(
                f"Confianza  : {detection['confidence']:.2f}"
            )

            self.get_logger().info(
                f"Posición   : "
                f"({detection['x']:.2f}, "
                f"{detection['y']:.2f}, "
                f"{detection['z']:.2f})"
            )

            self.get_logger().info(
                f"Frame      : {detection['frame_id']}"
            )


# Construye un mensaje DetectedObject a partir de la detección   
    def build_detected_object(self, detection):

        msg = DetectedObject()

        msg.class_name = detection["class_name"]

        msg.confidence = float(
            detection["confidence"]
        )

        msg.x = float(
            detection["x"]
        )

        msg.y = float(
            detection["y"]
        )

        msg.z = float(
            detection["z"]
        )

        msg.frame_id = detection["frame_id"]

        return msg

#Publisher
    # Publica el mensaje DetectedObject en el tópico ROS2
    def publish_detected_object(self, msg):

        self.publisher.publish(msg)

        self.get_logger().info(

            f"[VISION] "

            f"{msg.class_name} "

            f"({msg.confidence:.2f}) "

            f"[{msg.frame_id}]"

        )

# CLEANUP
    # Destruye el nodo y cierra las conexiones TCP
    def destroy_node(self):

        self.get_logger().info(
            "Cerrando Vision Bridge..."
        )

        try:

            if hasattr(self, "client_file") and self.client_file is not None:

                self.client_file.close()

                self.client_file = None

        except Exception:
            pass

        try:

            if self.client is not None:

                self.client.close()

                self.client = None

        except Exception:
            pass

        try:

            if self.server is not None:

                self.server.close()

                self.server = None

        except Exception:
            pass

        super().destroy_node()

# MAIN
def main(args=None):

    rclpy.init(args=args)

    node = VisionBridge()

    try:

        rclpy.spin(node)

    except KeyboardInterrupt:

        pass

    finally:

        node.destroy_node()

        rclpy.shutdown()

if __name__ == "__main__":

    main()