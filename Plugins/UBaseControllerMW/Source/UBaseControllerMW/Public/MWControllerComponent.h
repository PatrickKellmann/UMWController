// Copyright 2018, Institute for Artificial Intelligence - University of Bremen
// Author: Patrick Kellmann

#pragma once

#include "Engine.h"
#include "LevelEditor.h"
#include "Editor.h"
#include "Components/ActorComponent.h"
#include "MWControllerInterpolator.h"
#include "Runtime/Engine/Classes/PhysicsEngine/PhysicsConstraintComponent.h"
#include "MWControllerComponent.generated.h"

class MWControllerBoxComponentHandler;
class MWControllerBaseHandler;
class MWControllerWheelHandler;

#define DEVIATION_VALUE			(1.00f)
#define HALF_DISTANCE_DIVIDER	(2.f)
#define SCALE_FACTOR_BOXCOMP	(1.05f)
#define RADIUS_TO_DIAMETER		(2.f)
#define DIAMETER_TO_RADIUS		(2.f)
#define WHEEL_NUMBER_DIVIDER	(4.f)
#define SCALE_FACTOR_M_TO_CM	(100.f)
#define SCALE_FACTOR_CM_TO_M	(100.f)


//Channel for Collisions
#define ECC_MW					ECollisionChannel::ECC_GameTraceChannel1

/*
* Enum class that differs in X_Type and O_Type configuration.
*/
UENUM()
enum class EMWType : uint8
{
	MW_O_Type	UMETA(DisplayName = "MW_O_Type"),
	MW_X_Type	UMETA(DisplayName = "MW_X_Type")
};

/*
* UMWControllerComponent serves as an interface between the handlers, the required plugins and Unreal.
* Some calculations and configurations are performed in this class, as well as creating the handlers and the interpolator.
* Also stores data because handlers are not holding data except for their owner.
*/
UCLASS(EditInlineNew, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UBASECONTROLLERMW_API UMWControllerComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	/*
	* Sets default values for this component's properties
	*/
	UMWControllerComponent();

protected:

	/*
	* Starts after loading the Controller from Disk or before beginPlay. Generates new handlers and the interpolator and clean up.
	*/
	virtual void PostLoad();

	/*
	* Called when the game starts and starts the simulation.
	*
	*/
	virtual void BeginPlay() override;

	/*
	* Destroys the controller and cleans up. Deletes the handler and the interpolator.
	*
	* @param bPromoteChildren Indicates whether children should be promoted.
	*/
	virtual void DestroyComponent(bool bPromoteChildren) override;

	/*
	* TODO 
	*/
	void CreatePhysicsConstraints(UPhysicsConstraintComponent* PhyConsComp, AActor * ConstraintActor1, AActor * ConstraintActor2, UStaticMeshComponent * MeshComp);

	/*
	* TODO
	*/
	void DeletePhysicsConstraints();

#if WITH_EDITORONLY_DATA

	/*
	* Is called when editing at editor time.
	*
	* @param PropertyChangedEvent Object keeps the post edit change object.
	*/
	virtual void PostEditChangeProperty(FPropertyChangedEvent & PropertyChangedEvent) override;

	/*
	* Initializes the controller and starts the configuration.
	*
	*/
	virtual void PostInitProperties() override;

	/*
	* Makes the necessary settings on the controller, searches for duplications, assigns StaticmMeshComponents and starts calculations.
	* If problems occur, the setting is canceled and an error message is displayed. In difficult cases, the controller is destroyed directly.
	*
	* @return true if the controller could be set successfully. Otherwise false.
	*/
	bool AdjustMWControllerComponent();

#endif
public:

	/*
	* Called every frame. Checks every tick if the simulation conditions are met and then continues the simulation.
	* Otherwise the tick will be suspended.
	*/
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/*
	* Receiver for messages. Forwards the data to the interpolator.
	*
	* @param LinVel Lineare velocity input (x, y values are important).
	* @param AngVel Angular velocity input (z value is important).
	*/
	bool ReceiveROSMessage(FVector LinVel, FVector AngVel);

	/*
	* Getter for the Transform of the base.
	*
	* @return Transform of the base.
	*/
	const FTransform GetBaseTransform();

	/*
	* Getter for the angular velocity of the wheel.
	*
	* @return Angular velocity of the wheel.
	*/
	const float GetWheelLeftFrontAngularVelocity();

	/*
	* Getter for the angular velocity of the wheel.
	*
	* @return Angular velocity of the wheel.
	*/
	const float GetWheelRightFrontAngularVelocity();

	/*
	* Getter for the angular velocity of the wheel.
	*
	* @return Agular velocity of the wheel.
	*/
	const float GetWheelLeftRearAngularVelocity();

	/*
	* Getter for the angular velocity of the wheel.
	*
	* @return Angular velocity of the wheel.
	*/
	const float GetWheelRightRearAngularVelocity();

	/*
	* Getter for the longitudinal velocity of the base.
	*
	* @return Longitudinal velocity of the base.
	*/
	const float GetBaseLongitudinalVelocity();

	/*
	* Getter for the transversal velocity of the base.
	*
	* @return Transversal velocity of the base.
	*/
	const float GetBaseTransversalVelocity();

	/*
	* Getter for the angular velocity of the base.
	*
	* @return Angular velocity of the base.
	*/
	const float GetBaseAngularVelocity();

private:
	/*
	* Uses the new twist values. Starts turning the wheels and moving the base.
	*
	* @param MoveCom Vector with the new twist.
	*/
	void ExecuteCommandMovement(FVector MoveCom);

	/*
	* Checks the presence of all StaticMeshComponents (base, wheels).
	*
	* @return true if all StaticMeshComponents exist, otherwise false.
	*/
	bool AllStaticMeshComponentsExist();

public:
	// Wheel diameter is calculated.
	UPROPERTY(EditAnywhere, Category = "MW Details|Data", meta = (ToolTip = "Specification of the diameter of the wheels (average of the four). Can be calculated."))
		float WheelDiameterInCentimeter = 0.f;

	// Distance between front wheels is caluclated. 
	UPROPERTY(EditAnywhere, Category = "MW Details|Data", meta = (ToolTip = "Specification of the distance of the front wheels. Can be calculated."))
		float DistanceBetweenFrontwheels = 0.f;

	//  Distance between front and rear wheels is caluclated. 
	UPROPERTY(EditAnywhere, Category = "MW Details|Data", meta = (ToolTip = "Specification of the distance of the front to rear wheels. Can be calculated."))
		float HalfDistanceBetweenFrontRear = 0.f;

	// Combined value of HalfDistanceBetweenFrontRear and DistanceBetweenFrontwheels.
	UPROPERTY(EditAnywhere, Category = "MW Details|Data", meta = (ToolTip = "Gives the combined value of the distances. Can be calculated."))
		float CombinedDistanceValue = 0.f;

	// Default names for the StaticMeshComponents.
	// Change if you want to work with other names.
	UPROPERTY(VisibleAnywhere, Category = "MW Details|Name")
		FString MWRobotBaseActorName = "MW_Robot_Actor";
	UPROPERTY(VisibleAnywhere, Category = "MW Details|Name")
		FString BaseName = "MW_Base";
	UPROPERTY(VisibleAnywhere, Category = "MW Details|Name")
		FString WheelNameLeftFront = "Wheel_Left_Front";
	UPROPERTY(VisibleAnywhere, Category = "MW Details|Name")
		FString WheelNameRightFront = "Wheel_Right_Front";
	UPROPERTY(VisibleAnywhere, Category = "MW Details|Name")
		FString WheelNameLeftRear = "Wheel_Left_Rear";
	UPROPERTY(VisibleAnywhere, Category = "MW Details|Name")
		FString WheelNameRightRear = "Wheel_Right_Rear";

	//TODO Actor for wheel StaticMeshComponents
	UPROPERTY(EditAnywhere, Category = "MW Details|Actors")
		AActor* ActorWheelLF = nullptr;
	UPROPERTY(EditAnywhere, Category = "MW Details|Actors")
		AActor* ActorWheelRF = nullptr;
	UPROPERTY(EditAnywhere, Category = "MW Details|Actors")
		AActor* ActorWheelLR = nullptr;
	UPROPERTY(EditAnywhere, Category = "MW Details|Actors")
		AActor* ActorWheelRR = nullptr;

	TArray<AActor*> WheelActorList;

	// Standardname for wheel owner. 
	UPROPERTY(VisibleAnywhere, Category = "MW Details|Name")
		FString MWActorWheelLFName = "MWRobotWheelLF";
	UPROPERTY(VisibleAnywhere, Category = "MW Details|Name")
		FString MWActorWheelRFName = "MWRobotWheelRF";
	UPROPERTY(VisibleAnywhere, Category = "MW Details|Name")
		FString MWActorWheelLRName = "MWRobotWheelLR";
	UPROPERTY(VisibleAnywhere, Category = "MW Details|Name")
		FString MWActorWheelRRName = "MWRobotWheelRR";

	//TODO Physicsconstraints 
	UPROPERTY(EditAnywhere, Category = "MW Details|PhysicsConst")
		UPhysicsConstraintComponent* PhysicsConstBaseWheelLF = nullptr;
	UPROPERTY(EditAnywhere, Category = "MW Details|PhysicsConst")
		UPhysicsConstraintComponent* PhysicsConstBaseWheelRF = nullptr;
	UPROPERTY(EditAnywhere, Category = "MW Details|PhysicsConst")
		UPhysicsConstraintComponent* PhysicsConstBaseWheelLR = nullptr;
	UPROPERTY(EditAnywhere, Category = "MW Details|PhysicsConst")
		UPhysicsConstraintComponent* PhysicsConstBaseWheelRR = nullptr;

	// Owner of the mecanum wheel robot.
	UPROPERTY(VisibleAnywhere, Category = "MW Details|Actors")
		AActor* MWRobotBaseActor = nullptr;

	// StaticMeshComponents that are controlled as elements.
	UPROPERTY(VisibleInstanceOnly, Category = "MW Details|Mesh")
		UStaticMeshComponent* Base = nullptr;
	UPROPERTY(VisibleInstanceOnly, Category = "MW Details|Mesh")
		UStaticMeshComponent* WheelLeftFront = nullptr;
	UPROPERTY(VisibleInstanceOnly, Category = "MW Details|Mesh")
		UStaticMeshComponent* WheelRightFront = nullptr;
	UPROPERTY(VisibleInstanceOnly, Category = "MW Details|Mesh")
		UStaticMeshComponent* WheelLeftRear = nullptr;
	UPROPERTY(VisibleInstanceOnly, Category = "MW Details|Mesh")
		UStaticMeshComponent* WheelRightRear = nullptr;

	TArray<UStaticMeshComponent*> WheelList;

	// Stores the angular velocity of the wheels.
	float WheelLeftFrontAngularVelocity = 0.f;
	float WheelRightFrontAngularVelocity = 0.f;
	float WheelLeftRearAngularVelocity = 0.f;
	float WheelRightRearAngularVelocity = 0.f;

	// Indicates what type the robot is. Standard is O Type
	UPROPERTY(EditAnywhere, Category = "MW Details", meta = (ToolTip = "Indicates what type the robot is. Standard is O_Type. X_Type can not rotate."))
		EMWType MWType = EMWType::MW_O_Type;

	// Saves the BoxComponents that are created or assigned.
	UPROPERTY(EditAnywhere, Category = "MW Details|BoxComponents")
		UBoxComponent* BoxComponentBase = nullptr;
	UPROPERTY(EditAnywhere, Category = "MW Details|BoxComponents")
		UBoxComponent* BoxComponentLeftFront = nullptr;
	UPROPERTY(EditAnywhere, Category = "MW Details|BoxComponents")
		UBoxComponent* BoxComponentRightFront = nullptr;
	UPROPERTY(EditAnywhere, Category = "MW Details|BoxComponents")
		UBoxComponent* BoxComponentLeftRear = nullptr;
	UPROPERTY(EditAnywhere, Category = "MW Details|BoxComponents")
		UBoxComponent* BoxComponentRightRear = nullptr;

	// To change the angular movement polarity. 
	UPROPERTY(EditAnywhere, Category = "MW Details",
		meta = (ToolTip = "If the wheels move incorrectly during the angular movement of the base, the polarity can be changed here. This should only be 1.0 or -1.0!"))
		float PolarityForAngularMovement = -1.f;

	// Saves created BoxComponents that can not always be deleted cleanly via the editor so that you can remove them.
	TMap<FString, UBoxComponent*> CreatedBoxComponentsList;

	//Value for the interpolator for calculating the intermediate values. Depends on the fps of Unreal(calculation time in ms for a frame)
	UPROPERTY(EditAnywhere, Category = "MW Details")
		float CyleTimeInSeconds = 0.01666;

private:

	// Bool that indicates if the BoxComponents should be generated. 
	UPROPERTY(EditAnywhere, Category = "MW Details",
		meta = (ToolTip = "Specifies whether BoxComponents should be calculated automatically for collision. Default is On. Off leads to a delete of the components."))
		bool bUseCalcedBoxComponents = true;

	// Bool that indicates whether the distances should be recalculated.
	UPROPERTY(EditAnywhere, Category = "MW Details",
		meta = (ToolTip = "Specifies whether the calculation of the distances should happen again. Can be executed multiple times. In case of problems, messages appear."))
		bool bCalcWheelData = false;


	// Bool that indicates if the PhysicsConstraints should be deleted.
	UPROPERTY(EditAnywhere, Category = "MW Details",
		meta = (ToolTip = "TODO"))
		bool bUsePhysicsConstrains = true;

	// Stores the handler for tasks affecting the base.
	MWControllerBaseHandler* BaseHandler = nullptr;

	// Stores the handler for tasks affecting the wheels.
	MWControllerWheelHandler* WheelHandler = nullptr;

	// Stores the handler for tasks affecting the BoxComponents. 
	MWControllerBoxComponentHandler* BoxComponentHandler = nullptr;

	// Stores the interpolator for twist values. 
	MWControllerInterpolator* Interpolator = nullptr;

	// Bool that indicates whether the distances should be recalculated.
	UPROPERTY(EditAnywhere, Category = "MW Details",
		meta = (ToolTip = "Specifies whether a limit should be set for the velocity. Higher values are always overwritten with the specified maximum."))
		bool bUseVelocityLimits = false;

	// Limits to use. To be set in the editor.
	UPROPERTY(EditAnywhere, Category = "MW Details|Data", meta = (ToolTip = "Limit for Base Velocity (X value)"))
		float MaxXValue = 0.f;
	UPROPERTY(EditAnywhere, Category = "MW Details|Data", meta = (ToolTip = "Limit for Base Velocity (Y value)"))
		float MaxYValue = 0.f;
	UPROPERTY(EditAnywhere, Category = "MW Details|Data", meta = (ToolTip = "Limit for Base Velocity (Theta value)"))
		float MaxThetaValue = 0.f;
};

