from moveit_configs_utils import MoveItConfigsBuilder
from moveit_configs_utils.launches import generate_moveit_rviz_launch


def generate_launch_description():
    moveit_config = MoveItConfigsBuilder("panthera_ht_ros_description", package_name="panthera_ht_config_humble").to_moveit_configs()
    return generate_moveit_rviz_launch(moveit_config)
