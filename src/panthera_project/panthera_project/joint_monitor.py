import rclpy
from rclpy.node import Node
from sensor_msgs.msg import JointState

class JointMonitor(Node):

    def __init__(self):
        super().__init__("joint_monitor")

        self.get_logger().info("Joint Monitor iniciado")
        #Variable para almacenar el último estado de las articulaciones
        self.latest_joint_state = None
        
        self.subscription = self.create_subscription(
            JointState,
            "/joint_states",
            self.joint_state_callback,
            10,
        )
        #Timer cada 1 segundo
        self.timer = self.create_timer(
        1.0,
        self.print_joint_state
        )
    #Callback para imprimir el estado de las articulaciones
    def joint_state_callback(self, msg):
        self.latest_joint_state = msg
    
    #Función del timer
    def print_joint_state(self):

        if self.latest_joint_state is None:
            return

        self.get_logger().info("------ Estado actual ------")

        for name, position in zip(
            self.latest_joint_state.name,
            self.latest_joint_state.position
        ):

            self.get_logger().info(
                f"{name:8s}: {position:8.4f} rad"
            )

def main(args=None):

    rclpy.init(args=args) #inicia el cliente ROS2 en Python

    node = JointMonitor() #Crea el objeto nodo

    rclpy.spin(node) #Fundamental, Mantiene vivo el nodo

   #Liberan correctamente los recursos cuando finaliza la ejecución (por ejemplo, al pulsar Ctrl+C).
    node.destroy_node()

    rclpy.shutdown()


if __name__ == "__main__":
    main()