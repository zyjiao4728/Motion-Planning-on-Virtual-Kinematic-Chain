#include <vkc/env/arena_env.h>

using namespace trajopt;
using namespace tesseract;
using namespace tesseract_environment;
using namespace tesseract_scene_graph;
using namespace tesseract_collision;
using namespace tesseract_rosutils;

// URDF and SRDF file describes environment and robot
const std::string ENV_DESCRIPTION_PARAM = "env_description";
const std::string ENV_SEMANTIC_PARAM = "env_description_semantic";

// Link name defined as end effector
const std::string END_EFFECTOR_LINK = "end_effector_link";

// RVIZ service
const std::string GET_ENVIRONMENT_CHANGES_SERVICE = "get_tesseract_changes_rviz";
const std::string MODIFY_ENVIRONMENT_SERVICE = "modify_tesseract_rviz";

namespace vkc
{
ArenaEnv::ArenaEnv(ros::NodeHandle nh, bool plotting, bool rviz, int steps)
  : VKCEnvBasic(nh, plotting, rviz), steps_(steps)
{
  // Set Log Level
  util::gLogLevel = util::LevelInfo;

  loadRobotModel(ENV_DESCRIPTION_PARAM, ENV_SEMANTIC_PARAM, END_EFFECTOR_LINK);

  initTesseractConfig(MODIFY_ENVIRONMENT_SERVICE, GET_ENVIRONMENT_CHANGES_SERVICE);

  // set robot initial pose in scene graph
  setHomePose();

  ROS_INFO("Sucessfully load the robot model, now creating environment...");

  createEnvironment();

  ROS_INFO("Sucessfully create the environment, now creating optimization problem...");
}

bool ArenaEnv::createEnvironment()
{
  // double arena_x = 8.0;
  double arena_y = 8.0;
  // double arena_z = 2.1;

  vkc::BaseTable table0("table0", 0.8, 3, 1.6);
  table0.createObject();
  table0.createWorldJoint(Eigen::Vector4d(-0.5, arena_y / 2.0 - 0.5, 0, 0));
  table0.addToEnvironment(plot_tesseract_->getTesseract());
  table0.addToEnvironment(tesseract_->getTesseract());

  vkc::BaseCabinet cabinet0("cabinet0");
  cabinet0.createObject();
  cabinet0.createWorldJoint(Eigen::Vector4d(0, -arena_y / 2.0 + 1.7, 0, -1.57));
  cabinet0.inverseRootTip("world", cabinet0.getName() + "_handle_link");
  cabinet0.addToEnvironment(plot_tesseract_->getTesseract());
  cabinet0.addToEnvironment(tesseract_->getTesseract());

  vkc::BaseStick stick0("stick0", 0.5);
  stick0.createObject();
  stick0.createWorldJoint(Eigen::Vector4d(2.5, 1, 0.8, -1.57));
  stick0.inverseRootTip("world", stick0.getName() + "_stick_link");
  stick0.addToEnvironment(plot_tesseract_->getTesseract());
  stick0.addToEnvironment(tesseract_->getTesseract());

  vkc::BaseMarker marker0("marker0", 0.2);
  marker0.createObject();
  marker0.createWorldJoint(Eigen::Vector4d(-1, 4, 0.1, 1.57));
  marker0.inverseRootTip("world", marker0.getName() + "_marker_link");
  marker0.addToEnvironment(plot_tesseract_->getTesseract());
  marker0.addToEnvironment(tesseract_->getTesseract());

  vkc::BaseTable table1("table1", 0.72, 1, 2);
  table1.createObject();
  table1.createWorldJoint(Eigen::Vector4d(2.5, 1, 0, 0));
  table1.addToEnvironment(plot_tesseract_->getTesseract());
  table1.addToEnvironment(tesseract_->getTesseract());


  vkc::BaseWall obs1("obs1", 2, 3, 2.1);
  obs1.setColor(Eigen::Vector4d(0.5, 0.5, 0.5, 1));
  obs1.createObject();
  obs1.createWorldJoint(Eigen::Vector4d(2, -2, 0, 0));

  addAttachLocations(cabinet0.getAttachLocation());
  addAttachLocations(marker0.getAttachLocation());
  addAttachLocations(stick0.getAttachLocation());




  tesseract_->getTesseract()->getEnvironment()->addAllowedCollision("ur_arm_upper_arm_link", "ur_arm_wrist_1_link", "Never");
  tesseract_->getTesseract()->getEnvironment()->addAllowedCollision("ur_arm_upper_arm_link", "ur_arm_wrist_2_link", "Never");
  tesseract_->getTesseract()->getEnvironment()->addAllowedCollision("ur_arm_upper_arm_link", "ur_arm_wrist_3_link", "Never");
  tesseract_->getTesseract()->getEnvironment()->addAllowedCollision("ur_arm_forearm_link", "ur_arm_wrist_3_link", "Never");
  tesseract_->getTesseract()->getEnvironment()->addAllowedCollision("ur_arm_base_link", "ur_arm_upper_arm_link", "Never");
  tesseract_->getTesseract()->getEnvironment()->addAllowedCollision("cabinet0_cabinet_door_link", "ur_arm_wrist_3_link", "Never");
  tesseract_->getTesseract()->getEnvironment()->addAllowedCollision("stick0_stick_link", "ur_arm_wrist_3_link", "Never");
  tesseract_->getTesseract()->getEnvironment()->addAllowedCollision("ur_arm_ee_link", "ur_arm_upper_arm_link", "Never");
  tesseract_->getTesseract()->getEnvironment()->addAllowedCollision("ur_arm_ee_link", "ur_arm_forearm_link", "Never");

  tesseract_->getTesseract()->getEnvironment()->setLinkCollisionEnabled("cabinet0_knob1_link", false);
  tesseract_->getTesseract()->getEnvironment()->setLinkCollisionEnabled("cabinet0_knob2_link", false);
  tesseract_->getTesseract()->getEnvironment()->setLinkCollisionEnabled("cabinet0_handle_link", false);

  tesseract_->getTesseract()->getEnvironment()->addAllowedCollision("cabinet0_cabinet_bottom_link", "cabinet0_cabinet_door_link", "Never");

  tesseract_->getTesseract()->getEnvironment()->addAllowedCollision("cabinet0_cabinet_back_link", "cabinet0_cabinet_bottom_link", "Never");
  tesseract_->getTesseract()->getEnvironment()->addAllowedCollision("cabinet0_cabinet_back_link", "cabinet0_cabinet_left_link", "Never");
  tesseract_->getTesseract()->getEnvironment()->addAllowedCollision("cabinet0_cabinet_back_link", "cabinet0_cabinet_right_link", "Never");
  tesseract_->getTesseract()->getEnvironment()->addAllowedCollision("cabinet0_cabinet_back_link", "cabinet0_level1_link", "Never");
  tesseract_->getTesseract()->getEnvironment()->addAllowedCollision("cabinet0_cabinet_back_link", "cabinet0_level2_link", "Never");
  tesseract_->getTesseract()->getEnvironment()->addAllowedCollision("cabinet0_cabinet_back_link", "cabinet0_level3_link", "Never");
  tesseract_->getTesseract()->getEnvironment()->addAllowedCollision("cabinet0_cabinet_back_link", "cabinet0_top_link", "Never");
  tesseract_->getTesseract()->getEnvironment()->addAllowedCollision("cabinet0_cabinet_bottom_link", "cabinet0_cabinet_left_link", "Never");
  tesseract_->getTesseract()->getEnvironment()->addAllowedCollision("cabinet0_cabinet_bottom_link", "cabinet0_cabinet_right_link", "Never");
  tesseract_->getTesseract()->getEnvironment()->addAllowedCollision("cabinet0_level1_link", "cabinet0_cabinet_door_link", "Never");
  tesseract_->getTesseract()->getEnvironment()->addAllowedCollision("cabinet0_level2_link", "cabinet0_cabinet_door_link", "Never");
  tesseract_->getTesseract()->getEnvironment()->addAllowedCollision("cabinet0_level3_link", "cabinet0_cabinet_door_link", "Never");
  tesseract_->getTesseract()->getEnvironment()->addAllowedCollision("cabinet0_level1_link", "cabinet0_cabinet_right_link", "Never");
  tesseract_->getTesseract()->getEnvironment()->addAllowedCollision("cabinet0_level2_link", "cabinet0_cabinet_right_link", "Never");
  tesseract_->getTesseract()->getEnvironment()->addAllowedCollision("cabinet0_level3_link", "cabinet0_cabinet_right_link", "Never");
  tesseract_->getTesseract()->getEnvironment()->addAllowedCollision("cabinet0_top_link", "cabinet0_cabinet_right_link", "Never");
  tesseract_->getTesseract()->getEnvironment()->addAllowedCollision("cabinet0_level1_link", "cabinet0_cabinet_left_link", "Never");
  tesseract_->getTesseract()->getEnvironment()->addAllowedCollision("cabinet0_level2_link", "cabinet0_cabinet_left_link", "Never");
  tesseract_->getTesseract()->getEnvironment()->addAllowedCollision("cabinet0_level3_link", "cabinet0_cabinet_left_link", "Never");
  tesseract_->getTesseract()->getEnvironment()->addAllowedCollision("cabinet0_top_link", "cabinet0_cabinet_left_link", "Never");
  tesseract_->getTesseract()->getEnvironment()->addAllowedCollision("cabinet0_cabinet_left_link", "cabinet0_cabinet_door_link", "Never");
  tesseract_->getTesseract()->getEnvironment()->addAllowedCollision("cabinet0_cabinet_right_link", "cabinet0_cabinet_door_link", "Never");
  tesseract_->getTesseract()->getEnvironment()->addAllowedCollision("cabinet0_top_link", "cabinet0_cabinet_door_link", "Never");
  tesseract_->getTesseract()->getEnvironment()->addAllowedCollision("cabinet0_cabinet_door_link", "cabinet0_knob1_link", "Never");
  tesseract_->getTesseract()->getEnvironment()->addAllowedCollision("cabinet0_cabinet_door_link", "cabinet0_knob2_link", "Never");
  tesseract_->getTesseract()->getEnvironment()->addAllowedCollision("cabinet0_handle_link", "cabinet0_knob1_link", "Never");
  tesseract_->getTesseract()->getEnvironment()->addAllowedCollision("cabinet0_handle_link", "cabinet0_knob2_link", "Never");

  tesseract_->getTesseract()->getEnvironment()->setLinkCollisionEnabled("ur_arm_ee_link", false);
  tesseract_->getTesseract()->getEnvironment()->setLinkCollisionEnabled("user_rail_link", false);
  tesseract_->getTesseract()->getEnvironment()->setLinkCollisionEnabled("top_chassis_link", false);
  tesseract_->getTesseract()->getEnvironment()->setLinkCollisionEnabled("top_plate_link", false);
  tesseract_->getTesseract()->getEnvironment()->setLinkCollisionEnabled("front_bumper_link", false);
  tesseract_->getTesseract()->getEnvironment()->setLinkCollisionEnabled("front_left_wheel_link", false);
  tesseract_->getTesseract()->getEnvironment()->setLinkCollisionEnabled("front_right_wheel_link", false);
  tesseract_->getTesseract()->getEnvironment()->setLinkCollisionEnabled("rear_bumper_link", false);
  tesseract_->getTesseract()->getEnvironment()->setLinkCollisionEnabled("rear_left_wheel_link", false);
  tesseract_->getTesseract()->getEnvironment()->setLinkCollisionEnabled("rear_right_wheel_link", false);


  /////////
  plot_tesseract_->getTesseract()->getEnvironment()->addAllowedCollision("ur_arm_upper_arm_link", "ur_arm_wrist_1_link", "Never");
  plot_tesseract_->getTesseract()->getEnvironment()->addAllowedCollision("ur_arm_upper_arm_link", "ur_arm_wrist_2_link", "Never");
  plot_tesseract_->getTesseract()->getEnvironment()->addAllowedCollision("ur_arm_upper_arm_link", "ur_arm_wrist_3_link", "Never");
  plot_tesseract_->getTesseract()->getEnvironment()->addAllowedCollision("ur_arm_forearm_link", "ur_arm_wrist_3_link", "Never");
  plot_tesseract_->getTesseract()->getEnvironment()->addAllowedCollision("ur_arm_base_link", "ur_arm_upper_arm_link", "Never");
  plot_tesseract_->getTesseract()->getEnvironment()->addAllowedCollision("cabinet0_cabinet_door_link", "ur_arm_wrist_3_link", "Never");
  plot_tesseract_->getTesseract()->getEnvironment()->addAllowedCollision("stick0_stick_link", "ur_arm_wrist_3_link", "Never");
  plot_tesseract_->getTesseract()->getEnvironment()->addAllowedCollision("ur_arm_ee_link", "ur_arm_upper_arm_link", "Never");
  plot_tesseract_->getTesseract()->getEnvironment()->addAllowedCollision("ur_arm_ee_link", "ur_arm_forearm_link", "Never");

  plot_tesseract_->getTesseract()->getEnvironment()->setLinkCollisionEnabled("cabinet0_knob1_link", false);
  plot_tesseract_->getTesseract()->getEnvironment()->setLinkCollisionEnabled("cabinet0_knob2_link", false);
  plot_tesseract_->getTesseract()->getEnvironment()->setLinkCollisionEnabled("cabinet0_handle_link", false);

  plot_tesseract_->getTesseract()->getEnvironment()->addAllowedCollision("cabinet0_cabinet_bottom_link", "cabinet0_cabinet_door_link", "Never");

  plot_tesseract_->getTesseract()->getEnvironment()->addAllowedCollision("cabinet0_cabinet_back_link", "cabinet0_cabinet_bottom_link", "Never");
  plot_tesseract_->getTesseract()->getEnvironment()->addAllowedCollision("cabinet0_cabinet_back_link", "cabinet0_cabinet_left_link", "Never");
  plot_tesseract_->getTesseract()->getEnvironment()->addAllowedCollision("cabinet0_cabinet_back_link", "cabinet0_cabinet_right_link", "Never");
  plot_tesseract_->getTesseract()->getEnvironment()->addAllowedCollision("cabinet0_cabinet_back_link", "cabinet0_level1_link", "Never");
  plot_tesseract_->getTesseract()->getEnvironment()->addAllowedCollision("cabinet0_cabinet_back_link", "cabinet0_level2_link", "Never");
  plot_tesseract_->getTesseract()->getEnvironment()->addAllowedCollision("cabinet0_cabinet_back_link", "cabinet0_level3_link", "Never");
  plot_tesseract_->getTesseract()->getEnvironment()->addAllowedCollision("cabinet0_cabinet_back_link", "cabinet0_top_link", "Never");
  plot_tesseract_->getTesseract()->getEnvironment()->addAllowedCollision("cabinet0_cabinet_bottom_link", "cabinet0_cabinet_left_link", "Never");
  plot_tesseract_->getTesseract()->getEnvironment()->addAllowedCollision("cabinet0_cabinet_bottom_link", "cabinet0_cabinet_right_link", "Never");
  plot_tesseract_->getTesseract()->getEnvironment()->addAllowedCollision("cabinet0_level1_link", "cabinet0_cabinet_door_link", "Never");
  plot_tesseract_->getTesseract()->getEnvironment()->addAllowedCollision("cabinet0_level2_link", "cabinet0_cabinet_door_link", "Never");
  plot_tesseract_->getTesseract()->getEnvironment()->addAllowedCollision("cabinet0_level3_link", "cabinet0_cabinet_door_link", "Never");
  plot_tesseract_->getTesseract()->getEnvironment()->addAllowedCollision("cabinet0_level1_link", "cabinet0_cabinet_right_link", "Never");
  plot_tesseract_->getTesseract()->getEnvironment()->addAllowedCollision("cabinet0_level2_link", "cabinet0_cabinet_right_link", "Never");
  plot_tesseract_->getTesseract()->getEnvironment()->addAllowedCollision("cabinet0_level3_link", "cabinet0_cabinet_right_link", "Never");
  plot_tesseract_->getTesseract()->getEnvironment()->addAllowedCollision("cabinet0_top_link", "cabinet0_cabinet_right_link", "Never");
  plot_tesseract_->getTesseract()->getEnvironment()->addAllowedCollision("cabinet0_level1_link", "cabinet0_cabinet_left_link", "Never");
  plot_tesseract_->getTesseract()->getEnvironment()->addAllowedCollision("cabinet0_level2_link", "cabinet0_cabinet_left_link", "Never");
  plot_tesseract_->getTesseract()->getEnvironment()->addAllowedCollision("cabinet0_level3_link", "cabinet0_cabinet_left_link", "Never");
  plot_tesseract_->getTesseract()->getEnvironment()->addAllowedCollision("cabinet0_top_link", "cabinet0_cabinet_left_link", "Never");
  plot_tesseract_->getTesseract()->getEnvironment()->addAllowedCollision("cabinet0_cabinet_left_link", "cabinet0_cabinet_door_link", "Never");
  plot_tesseract_->getTesseract()->getEnvironment()->addAllowedCollision("cabinet0_cabinet_right_link", "cabinet0_cabinet_door_link", "Never");
  plot_tesseract_->getTesseract()->getEnvironment()->addAllowedCollision("cabinet0_top_link", "cabinet0_cabinet_door_link", "Never");
  plot_tesseract_->getTesseract()->getEnvironment()->addAllowedCollision("cabinet0_cabinet_door_link", "cabinet0_knob1_link", "Never");
  plot_tesseract_->getTesseract()->getEnvironment()->addAllowedCollision("cabinet0_cabinet_door_link", "cabinet0_knob2_link", "Never");
  plot_tesseract_->getTesseract()->getEnvironment()->addAllowedCollision("cabinet0_handle_link", "cabinet0_knob1_link", "Never");
  plot_tesseract_->getTesseract()->getEnvironment()->addAllowedCollision("cabinet0_handle_link", "cabinet0_knob2_link", "Never");

  plot_tesseract_->getTesseract()->getEnvironment()->setLinkCollisionEnabled("ur_arm_ee_link", false);
  plot_tesseract_->getTesseract()->getEnvironment()->setLinkCollisionEnabled("user_rail_link", false);
  plot_tesseract_->getTesseract()->getEnvironment()->setLinkCollisionEnabled("top_chassis_link", false);
  plot_tesseract_->getTesseract()->getEnvironment()->setLinkCollisionEnabled("top_plate_link", false);
  plot_tesseract_->getTesseract()->getEnvironment()->setLinkCollisionEnabled("front_bumper_link", false);
  plot_tesseract_->getTesseract()->getEnvironment()->setLinkCollisionEnabled("front_left_wheel_link", false);
  plot_tesseract_->getTesseract()->getEnvironment()->setLinkCollisionEnabled("front_right_wheel_link", false);
  plot_tesseract_->getTesseract()->getEnvironment()->setLinkCollisionEnabled("rear_bumper_link", false);
  plot_tesseract_->getTesseract()->getEnvironment()->setLinkCollisionEnabled("rear_left_wheel_link", false);
  plot_tesseract_->getTesseract()->getEnvironment()->setLinkCollisionEnabled("rear_right_wheel_link", false);


  if (rviz_)
  {
    // Now update rviz environment
    if (!sendRvizChanges(n_past_revisions_, tesseract_))
      return false;

    // if (!sendRvizChanges(n_past_plot_revisions_, plot_tesseract_))
    //   return false;
    // n_past_plot_revisions_ = plot_tesseract_->getTesseract()->getEnvironment()->getRevision();
    // ROS_INFO("tesseract revision: %lu, plot_tesseract_revision: %lu", n_past_revisions_, n_past_plot_revisions_);
    // std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

  }

  return true;
}
}  // namespace vkc
