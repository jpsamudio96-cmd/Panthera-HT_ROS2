from setuptools import setup

package_name = "panthera_camera_bridge"

setup(
    name=package_name,
    version="0.1.0",
    packages=[package_name],
    data_files=[
        (
            "share/ament_index/resource_index/packages",
            ["resource/" + package_name],
        ),
        (
            "share/" + package_name,
            ["package.xml"],
        ),
    ],
    install_requires=["setuptools"],
    zip_safe=True,
    maintainer="Jeffrey Pinto",
    maintainer_email="tu_correo",
    description="Panthera Camera Bridge",
    license="Apache-2.0",
    tests_require=["pytest"],
    entry_points={
        "console_scripts": [
            "camera_bridge_node = panthera_camera_bridge.camera_bridge_node:main",
        ],
    },
)