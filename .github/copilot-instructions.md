# Copilot instructions for this workspace

Purpose: quickly orient an AI coding agent to the ROS2 `colcon` workspace, the common coding patterns, and the developer workflows used here so suggestions and edits are immediately actionable.

Key facts
- **Workspace type**: ROS2 (colcon) multi-package workspace. Top-level folders: `src/`, `build/`, `install/`, `log/`.
- **Important packages**: `cpp01_topic` (C++ example nodes) and `base_interfaces_demo` (custom messages). See `src/cpp01_topic/src/demo03_talker_stu.cpp` for an example publisher and `base_interfaces_demo/msg/` for message definitions (e.g. `Student`).

Big picture architecture
- Packages are small, single-purpose ROS2 packages that communicate by topics and custom messages. Message definitions live in `base_interfaces_demo` and are consumed by other packages via `#include "base_interfaces_demo/msg/...hpp"`.
- Build artifacts are under `build/`, `install/`, and `log/` (standard colcon layout). Source changes require rebuilding with `colcon build` and then sourcing `install/setup.bash`.

Developer workflows (commands you can use)
- Build whole workspace: `colcon build` (from repo root).
- Build a single package: `colcon build --packages-select cpp01_topic`.
- Build up-to a package (ensure messages are available first): `colcon build --packages-up-to cpp01_topic`.
- Use symlink installs (fast iteration for C++): `colcon build --symlink-install`.
- Source environment before running nodes: `source install/setup.bash`.
- List executables in a package: `ros2 pkg executables <package_name>`.
- Run a node (example): `ros2 run cpp01_topic <executable_name>` — verify the exact executable with the previous command.
- Inspect topics: `ros2 topic list`, `ros2 topic echo /topic_stu`.
- Run tests: `colcon test` then `colcon test-result --verbose`.

Conventions & patterns to follow (concrete, repo-specific)
- C++ nodes derive from `rclcpp::Node`. Typical structure (see `src/cpp01_topic/src/demo03_talker_stu.cpp`):
  - members: `rclcpp::TimerBase::SharedPtr timer_;`, `rclcpp::Publisher<MyMsg>::SharedPtr publisher_;`, and a `size_t count_;`
  - constructor: call `this->create_publisher<Msg>("topic_name", 10)` and `this->create_wall_timer(500ms, ...)`.
  - message includes use the ament-generated header: `#include "base_interfaces_demo/msg/student.hpp"` and `using base_interfaces_demo::msg::Student;`.
- Use QoS depth `10` consistently for simple examples unless another QoS is required.
- Timer callbacks often publish simple messages and increment counters; prefer `std::bind(&Class::method, this)` for `create_wall_timer` callbacks to match existing style.
- CMakeLists: follow existing packages — use `add_executable(...)`, `ament_target_dependencies(...)`, and `install(TARGETS ... DESTINATION lib/${PROJECT_NAME})`.

Integration points and external dependencies
- ROS2 core (rclcpp, ros2cli). This repo depends on ament/rosidl-generated message headers from `base_interfaces_demo` — build order matters.
- Native tools: `colcon`, `ros2` CLI, and standard ROS2 debugging tools (`rqt_graph`, `ros2 topic`, `ros2 node`).

Editing guidance for AI agents (what to do and what to avoid)
- When adding or changing message types, ensure the message package (`base_interfaces_demo`) is built first and that dependent packages are rebuilt and re-sourced.
- Prefer small, focused edits: modify the node class in `src/cpp01_topic/src/*` and the package `CMakeLists.txt` together if you add an executable.
- When suggesting run/test instructions, include exact commands to build and source the workspace before running nodes.
- Avoid assuming executable names — verify with `ros2 pkg executables <pkg>` and read `CMakeLists.txt` to confirm `add_executable` targets.

Examples (copy/paste-ready)
- Build & run a single package:
  ```bash
  colcon build --packages-select cpp01_topic --symlink-install
  source install/setup.bash
  ros2 pkg executables cpp01_topic
  # then run the listed executable, e.g.:
  ros2 run cpp01_topic demo03_talker_stu
  ```
- Inspect topic used by C++ example:
  ```bash
  source install/setup.bash
  ros2 topic list
  ros2 topic echo /topic_stu
  ```

If something is missing
- If you need package-specific run commands or CI hooks, look for `CMakeLists.txt` and `package.xml` under each `src/<package>` to infer target names and dependencies.
- If builds fail because of missing messages, run `colcon build --packages-up-to base_interfaces_demo` then re-run the dependent package build.

Questions for the repo owner
- Do you use `--symlink-install` regularly for C++ rapid iteration? If so, I can prioritize suggestions that assume symlink installs.
- Are there recommended node executable names or launch files you prefer agents to use when proposing run commands?

---
Update request: reply with any missing patterns, special CI steps, or preferred runtime commands and I will iterate on this doc.
