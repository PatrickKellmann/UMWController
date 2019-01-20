# Unreal_MWController


# General

This software is a controller for Mecanum Wheel robots in the Unreal Engine. The controller is under development. The software includes a       ThirdParty plugin with [Reflexxes Motion Libraries Type II](http://www.reflexxes.ws/software/typeiirml/v1.2.6/docs/index.html). It can be added to an existing project. The robot in Unreal can be controlled via ROS. Some included plugins are from the  [IAI](https://github.com/robcog-iai) (Institute for Artificial Intelligence - University of Bremen).

# Quick Start
## 1. Integrate
* Integrate plugin folder into your project.
* Build your project

## 2. Using DemoMode (Map):
* Load Demomap.
* Found under UBaseControllerMW Content -> MWDemoMap.

#### 2.1 Set Game Mode.
* Create Blueprint version from the MyProjectGameModeBase (c ++ Classes -> MyProject)
* Set the PawnClass to "MWControllerDemoPawn" and PlayerController to "MWControllerController" with MyProjectGameModeBaseBLUEPRINT.

#### 2.2 If necessary: Reload and set the meshes.
* The sample meshes are located under UBaseControllerMW Content -> Meshes -> MW.
* For each mesh under Tags, set the following tag: "MWControllerComponentTag"
* For the three meshes (base, wheel left, wheel right) you may have to delete the collision (double-click on the Asset / Mesh -> Collision (next to Mesh) -> remove Collision) and create your own.

* Base = Box Collision
* This needs to be increased slightly. For Collision (next to Bounds) -> activate simple Collision. Click on the green box and adjust it slightly with the Unreal Editor Tool.

* Wheels = Sphere Collision
* No adjustment needed.

#### 2.3 Reassigning the Meshes (Consider the possible configurations of a Mecanum Wheel vehicle)
* Wheel Left Front   = Left Wheel
* Wheel Right Front  = Right Wheel
* Wheel Left Rear    = Right wheel
* Wheel Right Rear   = Left wheel

* If necessary, add the MWController again via AddComponent to the actor "MWRobotBaseActor".

#### 2.4 Play

## 3. Build your own MW.
#### 3.1 Use Empty Actors 
* One for the base and one for each wheel.

#### 3.2 Designation of Actors:
* Base              = MWRobotBaseActor
* Wheel Left Front  = MWRobotWheelLF
* Wheel Right Front = MWRobotWheelRF
* Wheel Left Rear   = MWRobotWheelLR
* Wheel Right Rear  = MWRobotWheelRR

#### 3.3 load and set the meshes.
* See 2.2.

#### 3.4 Position the wheels evenly.
* Too large deviations lead to suspension of the MWController functionality.

#### 3.5 Name the meshes as following:
* Base               = MW_Base
* Wheel Left Front   = Wheel_Left_Front
* Wheel Right Front  = Wheel_Right_Front
* Wheel Left Rear    = Wheel_Left_Rear
* Wheel Right Rear   = Wheel_Right_Rear

#### 3.6 Add a MW controller via "Add Component".
* This will automatically create all the remaining settings.
* Only one controller may be used at a time.
* Adding a new MWController deletes the old one.

#### 3.7 If necessary, load the config file for the controller.
* (see 2.3). 
* Or use a ROSMWController Subscriber for control over ROS.
* Use a ROSMWController Subscriber. 
* See for more information [ROSBridge]( https://github.com/robcog-iai/UROSBridge)

#### 3.8 Set Gamemode
* See 2.1.
* Pawn creates a camera.

#### 3.9 Play.

# Requirements
- Windows 
- UE 4.21
