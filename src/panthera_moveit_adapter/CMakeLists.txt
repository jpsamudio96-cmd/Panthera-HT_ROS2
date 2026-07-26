cmake_minimum_required(VERSION 3.8)
project(panthera_moveit_adapter)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# =========================
# Dependencies
# =========================

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(panthera_interfaces REQUIRED)

# =========================
# Include directories
# =========================

include_directories(
  include
)

set(PANTHERA_MOVEIT_ADAPTER_HEADERS
    include/panthera_moveit_adapter/moveit_adapter_node.hpp
    include/panthera_moveit_adapter/state_machine.hpp
    include/panthera_moveit_adapter/trajectory_library.hpp
)

set(PANTHERA_MOVEIT_ADAPTER_SOURCES
    src/main.cpp
    src/moveit_adapter_node.cpp
)

# =========================
# Executable
# =========================

add_executable(
    moveit_adapter_node
    ${PANTHERA_MOVEIT_ADAPTER_SOURCES}
)

ament_target_dependencies(
  moveit_adapter_node
  rclcpp
  panthera_interfaces
)

# =========================
# Install
# =========================

install(
  TARGETS moveit_adapter_node
  DESTINATION lib/${PROJECT_NAME}
)

install(
  DIRECTORY include/
  DESTINATION include
)

# =========================
# Testing
# =========================

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  set(ament_cmake_copyright_FOUND TRUE)
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()
