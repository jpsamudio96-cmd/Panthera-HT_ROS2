from launch import LaunchDescription
from launch.actions import OpaqueFunction

from launch_ros.actions import Node
from launch_ros.substitutions import FindPackageShare

from moveit_configs_utils import MoveItConfigsBuilder

import os


def launch_setup(context, *args, **kwargs):

    panthera_config_path = FindPackageShare(
        "panthera_ht_config"
    ).perform(context)

    moveit_config = (
        MoveItConfigsBuilder(
            "panthera_ht_ros_description",
            package_name="panthera_ht_config"
        )
        .robot_description_semantic(
            file_path=os.path.join(
                panthera_config_path,
                "config",
                "panthera_ht_ros_description.srdf"
            )
        )
        .to_moveit_configs()
    )

    backend_node = Node(
        package="panthera_motion_backend",
        executable="motion_backend_node",
        output="screen",
        parameters=[
            moveit_config.to_dict(),
            {
                "use_sim_time": False,
            },
        ],
    )

    return [backend_node]


def generate_launch_description():

    return LaunchDescription(
        [
            OpaqueFunction(
                function=launch_setup
            )
        ]
    )