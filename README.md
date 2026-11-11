# Mobile Robotics — Odometry, SLAM & Autonomous Navigation

**ROS 2 · C++ · Mobile robotics · SLAM Toolbox · Nav2**

Selected university coursework from **Politecnico di Milano**, presented as a portfolio of two ROS 2 projects. The repository contains the project source code and a curated set of configuration files. It is **not a ready-to-run robot distribution**; the original course setup, required dependencies, and robot/simulation data are not included.

## 1. Odometry and TF evaluation

The `first_project` ROS 2 package processes status messages from an AgileX Bunker Pro mobile robot:

- Integrates linear and angular velocity measurements to estimate planar robot position and heading
- Publishes the estimate on `/project_odom` and broadcasts the `odom → base_link2` TF transform
- Provides a custom odometry-reset service
- Compares estimated and reference transforms and publishes positioning error and travelled distance

**Main files:** [odometer.cpp](first_project/src/odometer.cpp), [tf_error.cpp](first_project/src/tf_error.cpp).

## 2. SLAM and navigation

The `second_project` ROS 2 package is built around:

- Point cloud conversion to a `/scan` LaserScan topic
- Mapping with asynchronous **SLAM Toolbox**
- Navigation with **Nav2** (localization, path planning and control)
- Sequential navigation goals loaded from a CSV file and sent via the `NavigateToPose` action

**Main files:** [mapping.launch.py](second_project/launch/mapping.launch.py), [navigation.launch.py](second_project/launch/navigation.launch.py), [goal_publisher.cpp](second_project/src/goal_publisher.cpp).

## Technologies

| Area | Technologies |
| --- | --- |
| Programming | C++, Python |
| Robotics | ROS 2, TF2, odometry, SLAM, Nav2 |
| Sensing | Point cloud to LaserScan |
| Visualization | RViz |
| Build system | CMake, ament |

## Structure

```text
first_project/
  src/                  Odometry and transform-error nodes
  msg/ and srv/         Custom ROS 2 interfaces
  launch/ and rviz/     Launch and visualization configuration
second_project/
  src/                  Navigation goal publisher
  launch/               Mapping and navigation launch files
  config/               Nav2 parameters
  csv/                  Example navigation goals
```

## Setup notes and limitations

1. A compatible **ROS 2** installation with the required packages is needed, including `rclcpp`, `tf2`, `slam_toolbox`, `pointcloud_to_laserscan` and **Nav2**.
2. The odometry project depends on `bunker_msgs` (AgileX/Weston Robot message definitions), **not redistributed here**. Install an appropriately licensed copy separately.
3. The mapping project requires a compatible point cloud, odometry/TF data, and robot or simulation environment.
4. For navigation, provide a **valid saved map YAML and occupancy-grid image** from your own environment. Course map artifacts and incomplete RViz configuration are not included. See the launch-file comments.
5. The code and configurations are preserved from coursework with small packaging/documentation adjustments; **building and end-to-end execution have not been independently verified** for this portfolio version.

## Credits

Developed in the context of university robotics coursework at Politecnico di Milano. The portfolio highlights work completed within the course; associated exercises may have involved other students and course-provided components.

*No software license is granted by this repository unless separately specified in an individual package. Confirm applicable university and collaborator permissions before reusing or redistributing any material.*
