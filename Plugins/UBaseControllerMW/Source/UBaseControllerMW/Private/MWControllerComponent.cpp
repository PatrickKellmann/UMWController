// Copyright 2018, Institute for Artificial Intelligence - University of Bremen
// Author: Patrick Kellmann

#include "MWControllerComponent.h"
#include "MWControllerWheelHandler.h"
#include "MWControllerConstraintHandler.h"
#include "MWControllerBaseHandler.h"


#if WITH_EDITOR

// Sets default values for this component's properties
UMWControllerComponent::UMWControllerComponent()
{
	// Sets this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickGroup = TG_PrePhysics;
}


// Begins init. 
void UMWControllerComponent::PostInitProperties()
{
	Super::PostInitProperties();
	// Default objects or loaded objects should not be edited.
	if (this && !this->IsDefaultSubobject() && !this->HasAnyFlags(EObjectFlags::RF_WasLoaded))
	{
		if (!AdjustMWControllerComponent())
		{
			UE_LOG(LogTemp, Error,
				TEXT("[%s][%d] AdjustMWControllerComponent was false. MWControllerComponent will be deleted."),
				TEXT(__FUNCTION__), __LINE__);

			GEditor->OnModalMessageDialog(EAppMsgType::Ok,
				FText::FromString("Adjust of the MWControllerComponent failed. MWControllerComponent will be deleted."),
				FText::FromString("MW Error Dialog"));

			this->DestroyComponent(false);
		}
		// Set so that a MWController does not want to reload everything at BeginPlay. 
		// At BeginPlay the Actor lacks the StaticMeshComponents which causes errors.
		this->SetFlags(EObjectFlags::RF_WasLoaded);
	}
}


// Configuration of the MWController.
bool UMWControllerComponent::AdjustMWControllerComponent()
{
	MWRobotBaseActor = this->GetOwner();

	// We will see if there is already a MWController on this Actor. That would cause Unreal to crash so we delete it and keep the new one.
	if (MWRobotBaseActor && MWRobotBaseActor->IsValidLowLevel())
	{
		TArray<UMWControllerComponent*> ActorComponentList;
		MWRobotBaseActor->GetComponents<UMWControllerComponent>(ActorComponentList);

		for (auto ActComp : ActorComponentList)
		{
			if (ActComp->ComponentHasTag("MWControllerComponentTag"))
			{
				// Log and Dialog message
				UE_LOG(LogTemp, Warning,
					TEXT("[%s][%d] More than one MWControllerComponent detected, old one will be deleted. Please use only one MWControllerComponent at the time."),
					TEXT(__FUNCTION__), __LINE__);

				GEditor->OnModalMessageDialog(EAppMsgType::Ok,
					FText::FromString("More than one MWControllerComponent detected, old one will be deleted. Please use only one MWControllerComponent at the time."),
					FText::FromString("MW Error Dialog"));

				// Destroys the found MWController. 
				if (ActComp && this != ActComp)
				{
					ActComp->DestroyComponent(false);
				}
				break;
			}
		}

		// Creates a PhyMater. prevents friction.
		CollisionMaterial = NewObject<UPhysicalMaterial>(this, TEXT("MW_CollisionMaterial"));
		CollisionMaterial->bOverrideFrictionCombineMode = true;
		CollisionMaterial->Friction = 0.f;
		CollisionMaterial->FrictionCombineMode = EFrictionCombineMode::Min;
		CollisionMaterial->Restitution = 0.f;
		CollisionMaterial->RestitutionCombineMode = EFrictionCombineMode::Min;
		CollisionMaterial->bOverrideRestitutionCombineMode = true;
		CollisionMaterial->Density = 0.f;


		// Gets wheels and the base as StaticMeshComponents.
		TArray<UStaticMeshComponent*> MWRobotComponents;
		MWRobotBaseActor->GetComponents<UStaticMeshComponent>(MWRobotComponents);

		for (auto BaseMesh : MWRobotComponents)
		{
			if (BaseMesh->ComponentHasTag("MWStaticMeshComponentTag") && BaseMesh->GetName().Equals(BaseName))
			{
				Base = BaseMesh;
				Base->PrimaryComponentTick.TickGroup = TG_PrePhysics;
				Base->SetCollisionProfileName(TEXT("BlockAll"));
				Base->SetSimulatePhysics(true);
				Base->SetEnableGravity(true);

				Base->SetMassOverrideInKg(NAME_None, 1000.f);
				Base->SetPhysMaterialOverride(CollisionMaterial);
			}
		}

		// Get the actors who holds the wheels. 
		TArray<AActor*> HelperActorList;
		UGameplayStatics::GetAllActorsOfClass(this->GetWorld(), AActor::StaticClass(), HelperActorList);

		for (auto WheelAct : HelperActorList)
		{
			// Look for the default names. "Contains" is used because Unreal changes the name in some cases.
			if (WheelAct->GetName().Contains(MWActorWheelLFName))
			{
				if (WheelAct && WheelAct->IsValidLowLevel())
				{
					ActorWheelLF = WheelAct;
					ActorWheelLF->SetTickGroup(TG_PrePhysics);
					WheelActorList.Add(WheelAct);
				}
			}
			else if (WheelAct->GetName().Contains(MWActorWheelRFName))
			{
				if (WheelAct && WheelAct->IsValidLowLevel())
				{
					ActorWheelRF = WheelAct;
					ActorWheelRF->SetTickGroup(TG_PrePhysics);
					WheelActorList.Add(WheelAct);
				}
			}
			else if (WheelAct->GetName().Contains(MWActorWheelLRName))
			{
				if (WheelAct && WheelAct->IsValidLowLevel())
				{
					ActorWheelLR = WheelAct;
					ActorWheelLR->SetTickGroup(TG_PrePhysics);
					WheelActorList.Add(WheelAct);
				}
			}
			else if (WheelAct->GetName().Contains(MWActorWheelRRName))
			{
				if (WheelAct && WheelAct->IsValidLowLevel())
				{
					ActorWheelRR = WheelAct;
					ActorWheelRR->SetTickGroup(TG_PrePhysics);
					WheelActorList.Add(WheelAct);
				}
			}
		}

		// Find and set the wheels and meshes.
		for (auto WheelActor : WheelActorList)
		{
			TArray<UStaticMeshComponent*> WheelStaticMeshComponentList;
			WheelActor->GetComponents<UStaticMeshComponent>(WheelStaticMeshComponentList);
			for (auto WheelMesh : WheelStaticMeshComponentList)
			{
				if (WheelMesh->ComponentHasTag("MWStaticMeshComponentTag"))
				{
					WheelMesh->PrimaryComponentTick.TickGroup = TG_PrePhysics;
					WheelMesh->SetCollisionProfileName(TEXT("BlockAll"));
					WheelMesh->SetSimulatePhysics(true);
					WheelMesh->SetEnableGravity(true);
					WheelMesh->SetMassOverrideInKg(NAME_None, 1.f);
					WheelMesh->SetPhysMaterialOverride(CollisionMaterial);
				}

				if (WheelMesh->GetName().Contains(WheelNameLeftFront))
				{
					WheelLeftFront = WheelMesh;
					PhysicsConstBaseWheelLF = NewObject<UPhysicsConstraintComponent>(this, "PhysicsConstBaseWheelLF");
					ConstraintList.Add(FConstraintStruct(PhysicsConstBaseWheelLF, MWRobotBaseActor, ActorWheelLF, WheelLeftFront));
				}
				else if (WheelMesh->GetName().Contains(WheelNameRightFront))
				{
					WheelRightFront = WheelMesh;
					PhysicsConstBaseWheelRF = NewObject<UPhysicsConstraintComponent>(this, "PhysicsConstBaseWheelRF");
					ConstraintList.Add(FConstraintStruct(PhysicsConstBaseWheelRF, MWRobotBaseActor, ActorWheelRF, WheelRightFront));
				}
				else if (WheelMesh->GetName().Contains(WheelNameLeftRear))
				{
					WheelLeftRear = WheelMesh;
					PhysicsConstBaseWheelLR = NewObject<UPhysicsConstraintComponent>(this, "PhysicsConstBaseWheelLR");
					ConstraintList.Add(FConstraintStruct(PhysicsConstBaseWheelLR, MWRobotBaseActor, ActorWheelLR, WheelLeftRear));
				}
				else if (WheelMesh->GetName().Contains(WheelNameRightRear))
				{
					WheelRightRear = WheelMesh;
					PhysicsConstBaseWheelRR = NewObject<UPhysicsConstraintComponent>(this, "PhysicsConstBaseWheelRR");
					ConstraintList.Add(FConstraintStruct(PhysicsConstBaseWheelRR, MWRobotBaseActor, ActorWheelRR, WheelRightRear));
				}
			}
		}

		// Checks if all 5 elements are present.
		if (AllStaticMeshComponentsExist())
		{
			// Creates the handlers and the interpolator for the calculations
			BaseHandler = new MWControllerBaseHandler(this);
			WheelHandler = new MWControllerWheelHandler(this);
			ConstraintHandler = new MWControllerConstraintHandler(this);
			Interpolator = new MWControllerInterpolator(CyleTimeInSeconds);

			if (!ConstraintHandler->SetupPhysicsConstraints())
			{
				UE_LOG(LogTemp, Error,
					TEXT("[%s][%d] Something went wrong (creating the constraints)"), TEXT(__FUNCTION__), __LINE__);
			}

			// Sees if the wheels are evenly spaced, also calculates the distances
			if (WheelHandler->IsDistanceBetweenWheelsValid())
			{
				TPair<float, float> CheckValues = WheelHandler->CalcDistanceBetweenWheels();
				DistanceBetweenFrontwheels = CheckValues.Key;
				HalfDistanceBetweenFrontRear = CheckValues.Value;
				CombinedDistanceValue = DistanceBetweenFrontwheels + HalfDistanceBetweenFrontRear;
			}
			else
			{
				UE_LOG(LogTemp, Error,
					TEXT("[%s][%d] The distance between the wheels was wrong"), TEXT(__FUNCTION__), __LINE__);

				GEditor->OnModalMessageDialog(EAppMsgType::Ok,
					FText::FromString("The distance between the wheels was wrong. Please fix it before BeginPlay otherwise the controller will be deleted."),
					FText::FromString("MW Error Dialog"));
			}

			//Sets the tag with which the MWController can be found from outers. 
			this->ComponentTags.Add("MWControllerComponentTag");

			//Looks how the diameter of the wheels is.
			WheelDiameterInCentimeter = WheelHandler->CalcWheelDiameterInCentimeter();
			if (FMath::IsNearlyEqual(WheelDiameterInCentimeter, -1.f))
			{
				UE_LOG(LogTemp, Error,
					TEXT("[%s][%d] WheelDiameterInCentimeter was wrong. Please fix that."), TEXT(__FUNCTION__), __LINE__);

				GEditor->OnModalMessageDialog(EAppMsgType::Ok,
					FText::FromString("WheelDiameterInCentimeter was wrong. Please fix that."),
					FText::FromString("MW Error Dialog"));
			}
		}
		else
		{
			GEditor->OnModalMessageDialog(EAppMsgType::Ok,
				FText::FromString("Not all StaticMeshComponents could be found. Study the log files, look which one are missing and fix that. MWcontroller will be destroyed."),
				FText::FromString("MW Error Dialog"));

			this->DestroyComponent(false);
		}
		return true;
	}
	return false;
}

// Is called when changes appear.
void UMWControllerComponent::PostEditChangeProperty(FPropertyChangedEvent & PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	// Gets the changed property and member names
	FName PropertyName = PropertyChangedEvent.GetPropertyName();

	if (PropertyName == FName("bUsePhysicsConstrains"))
	{
		// Creates or deletes the PhysicsConstraints.
		if (bUsePhysicsConstrains)
		{
			PhysicsConstBaseWheelLF = NewObject<UPhysicsConstraintComponent>(this, "PhysicsConstBaseWheelLF");
			PhysicsConstBaseWheelRF = NewObject<UPhysicsConstraintComponent>(this, "PhysicsConstBaseWheelRF");
			PhysicsConstBaseWheelLR = NewObject<UPhysicsConstraintComponent>(this, "PhysicsConstBaseWheelLR");
			PhysicsConstBaseWheelRR = NewObject<UPhysicsConstraintComponent>(this, "PhysicsConstBaseWheelRR");
			ConstraintList.Add(FConstraintStruct(PhysicsConstBaseWheelLF, MWRobotBaseActor, ActorWheelLF, WheelLeftFront));
			ConstraintList.Add(FConstraintStruct(PhysicsConstBaseWheelRF, MWRobotBaseActor, ActorWheelRF, WheelRightFront));
			ConstraintList.Add(FConstraintStruct(PhysicsConstBaseWheelLR, MWRobotBaseActor, ActorWheelLR, WheelLeftRear));
			ConstraintList.Add(FConstraintStruct(PhysicsConstBaseWheelRR, MWRobotBaseActor, ActorWheelRR, WheelRightRear));

			// Creates the constraints for the wheels. 
			if (!ConstraintHandler->SetupPhysicsConstraints())
			{
				UE_LOG(LogTemp, Error,
					TEXT("[%s][%d] Something went wrong (creating the constraints)"), TEXT(__FUNCTION__), __LINE__);
			}

		}
		else
		{
			ConstraintHandler->DeletePhysicsConstraints();
		}
	}
	else if (PropertyName == FName("CyleTimeInSeconds"))
	{
		//For changes, the interpolator must be reset.
		Interpolator->~MWControllerInterpolator();
		Interpolator = nullptr;
		Interpolator = new MWControllerInterpolator(CyleTimeInSeconds);
	}
	else if (PropertyName == FName("bUseWheelRotatio"))
	{
		if (bUseWheelRotation)
		{
			bUseWheelRotation = false;
		}
		else
		{
			bUseWheelRotation = true;
		}
	}
	else if (PropertyName == FName("bCalcWheelData"))
	{
		if (bCalcWheelData)
		{
			// Checks if all 5 elements are present.
			if (AllStaticMeshComponentsExist())
			{
				// Sees if the wheels are evenly spaced, also calculates the distances
				if (WheelHandler->IsDistanceBetweenWheelsValid())
				{
					TPair<float, float> CheckValues = WheelHandler->CalcDistanceBetweenWheels();
					DistanceBetweenFrontwheels = CheckValues.Key;
					HalfDistanceBetweenFrontRear = CheckValues.Value;
					CombinedDistanceValue = DistanceBetweenFrontwheels + HalfDistanceBetweenFrontRear;
				}
				else
				{
					UE_LOG(LogTemp, Error,
						TEXT("[%s][%d] The distance between the wheels was wrong"), TEXT(__FUNCTION__), __LINE__);

					GEditor->OnModalMessageDialog(EAppMsgType::Ok,
						FText::FromString("The distance between the wheels was wrong. Please fix it before BeginPlay otherwise the controller will be deleted."),
						FText::FromString("MW Error Dialog"));
				}

				//Looks how the diameter of the wheels is.
				WheelDiameterInCentimeter = WheelHandler->CalcWheelDiameterInCentimeter();
				if (FMath::IsNearlyEqual(WheelDiameterInCentimeter, -1.f))
				{
					UE_LOG(LogTemp, Error,
						TEXT("[%s][%d] WheelDiameterInCentimeter was wrong. Please fix that."), TEXT(__FUNCTION__), __LINE__);

					GEditor->OnModalMessageDialog(EAppMsgType::Ok,
						FText::FromString("WheelDiameterInCentimeter was wrong. Please fix that."),
						FText::FromString("MW Error Dialog"));
				}
			}
		}
	}
}
#endif

// Called when loading from disk or to BeginPlay to fix things. 
void UMWControllerComponent::PostLoad()
{
	Super::PostLoad();

	if (this && this->MWRobotBaseActor)
	{
		// Creates the handlers and the interpolator since these are not stored by Unreal.
		if (!BaseHandler && !WheelHandler)
		{
			BaseHandler = new MWControllerBaseHandler(this);
			WheelHandler = new MWControllerWheelHandler(this);
			ConstraintHandler = new MWControllerConstraintHandler(this);
			Interpolator = new MWControllerInterpolator(CyleTimeInSeconds);
		}
	}
}

// Called when the game starts
void UMWControllerComponent::BeginPlay()
{
	Super::BeginPlay();
}

// Destroys MWController and cleans up.
void UMWControllerComponent::DestroyComponent(bool bPromoteChildren)
{
	Super::DestroyComponent(bPromoteChildren);

	// Destroys the handlers and the interpolator
	if (BaseHandler)
	{
		BaseHandler->~MWControllerBaseHandler();
	}
	if (WheelHandler)
	{
		WheelHandler->~MWControllerWheelHandler();
	}
	if (ConstraintHandler)
	{
		ConstraintHandler->~MWControllerConstraintHandler();
	}
	if (Interpolator)
	{
		Interpolator->~MWControllerInterpolator();
	}
	if (PhysicsConstBaseWheelLF && PhysicsConstBaseWheelRF && PhysicsConstBaseWheelLR && PhysicsConstBaseWheelRR)
	{
		ConstraintHandler->DeletePhysicsConstraints();
	}
}

// Called every frame
void UMWControllerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);


	// Sees if distance has become faulty due to the deviations.
	if (!WheelHandler->IsDistanceBetweenWheelsValid())
	{
		this->SetComponentTickEnabled(false);
		UE_LOG(LogTemp, Error,
			TEXT("[%s][%d]Distance of the wheels has deviated too far (difference> 1.0). Tick of UMWControllerComponent will be turn off."),
			TEXT(__FUNCTION__), __LINE__);
	}

	// Turns off the tick when problems occur, as this is safer than to destroy.
	if (!MWRobotBaseActor || !MWRobotBaseActor->IsValidLowLevel() || !AllStaticMeshComponentsExist())
	{

		this->SetComponentTickEnabled(false);
		UE_LOG(LogTemp, Error,
			TEXT("[%s][%d]. Tick of UMWControllerComponent will be turned off."),
			TEXT(__FUNCTION__), __LINE__);
	}
	else
	{

		// Gives the pose - Location X, Y - Orientation Yaw as double 
		Interpolator->set_current_pose(double(Base->GetComponentLocation().X), double(Base->GetComponentLocation().Y), double(Base->GetComponentRotation().Yaw));

		FVector NextTwist = Interpolator->get_next_twist();

		// Starts with the implementation of the simulation.
		ExecuteCommandMovement(FVector(NextTwist.X, NextTwist.Y, NextTwist.Z));

		// Sets the actor to the same transform, so that he comes along.
		MWRobotBaseActor->SetActorTransform(Base->GetComponentTransform());
	}
}


// Interface for ROS (and PlayerController) for control.
bool UMWControllerComponent::ReceiveROSMessage(FVector LinVel, FVector AngVel)
{

	double XVal = LinVel.X;
	double YVal = LinVel.Y;
	double Theta = AngVel.Z;

	if (Interpolator)
	{
		//If limits are to be used, they are used. Standard is 0.f.
		if (bUseVelocityLimits)
		{
			if (XVal > MaxXValue)
			{
				XVal = MaxXValue;
			}
			else if (XVal < 0.f && XVal < (MaxXValue *-1.f))
			{
				XVal = MaxXValue * -1.f;
			}

			if (YVal > MaxYValue)
			{
				YVal = MaxYValue;
			}
			else if (YVal < 0.f && YVal < (MaxYValue *-1.f))
			{
				YVal = MaxYValue * -1.f;
			}

			if (Theta > MaxThetaValue)
			{
				Theta = MaxThetaValue;
			}
			else if (Theta < 0.f && Theta < (MaxThetaValue *-1.f))
			{
				Theta = MaxThetaValue * -1.f;
			}
		}
		Interpolator->set_target_twist(XVal, YVal, Theta);

		return true;
	}
	return false;
}

// Does the work for the simulation. 
void UMWControllerComponent::ExecuteCommandMovement(FVector MoveCom)
{
	BaseHandler->SetBaseLinearVelocity(MoveCom);
	BaseHandler->SetBaseAngularVelocity(MoveCom);

	if (bUseWheelRotation)
	{
		WheelHandler->SetupWheelsMovement(MoveCom);
	}
}

// Function looks if all elements are present and indicates which ones are missing.
bool UMWControllerComponent::AllStaticMeshComponentsExist()
{
	bool bMissingMeshComp = false;

	if (!Base)
	{
		UE_LOG(LogTemp, Error, TEXT("[%s][%d]. %s was not found."), TEXT(__FUNCTION__), __LINE__, *BaseName);
		bMissingMeshComp = true;
	}
	if (!WheelRightRear)
	{
		UE_LOG(LogTemp, Error, TEXT("[%s][%d]. %s was not found."), TEXT(__FUNCTION__), __LINE__, *WheelNameRightRear);
		bMissingMeshComp = true;
	}
	if (!WheelLeftRear)
	{
		UE_LOG(LogTemp, Error, TEXT("[%s][%d]. %s was not found."), TEXT(__FUNCTION__), __LINE__, *WheelNameLeftRear);
		bMissingMeshComp = true;
	}
	if (!WheelRightFront)
	{
		UE_LOG(LogTemp, Error, TEXT("[%s][%d]. %s was not found."), TEXT(__FUNCTION__), __LINE__, *WheelNameRightFront);
		bMissingMeshComp = true;
	}
	if (!WheelLeftFront)
	{
		UE_LOG(LogTemp, Error, TEXT("[%s][%d]. %s was not found."), TEXT(__FUNCTION__), __LINE__, *WheelNameLeftFront);
		bMissingMeshComp = true;
	}

	return bMissingMeshComp ? false : true;
}

// Getter for base transform.
const FTransform UMWControllerComponent::GetBaseTransform()
{
	return Base->GetComponentTransform();
}

// Getter for Velocity.
const float UMWControllerComponent::GetWheelLeftFrontAngularVelocity()
{
	FRotator Rot = WheelLeftFront->GetComponentRotation();
	FVector Vel = Rot.UnrotateVector(WheelLeftFront->GetPhysicsLinearVelocity());
	return Vel.Y;
}

// Getter for Velocity.
const float UMWControllerComponent::GetWheelRightFrontAngularVelocity()
{
	FRotator Rot = WheelRightFront->GetComponentRotation();
	FVector Vel = Rot.UnrotateVector(WheelRightFront->GetPhysicsLinearVelocity());
	return Vel.Y;
}

// Getter for Velocity.
const float UMWControllerComponent::GetWheelLeftRearAngularVelocity()
{
	FRotator Rot = WheelLeftRear->GetComponentRotation();
	FVector Vel = Rot.UnrotateVector(WheelLeftRear->GetPhysicsLinearVelocity());
	return Vel.Y;
}

// Getter for Velocity.
const float UMWControllerComponent::GetWheelRightRearAngularVelocity()
{
	FRotator Rot = WheelRightRear->GetComponentRotation();
	FVector Vel = Rot.UnrotateVector(WheelRightRear->GetPhysicsLinearVelocity());
	return Vel.Y;
}

// Gets the LongitudinalVelocity of the Base. 
const float UMWControllerComponent::GetBaseLongitudinalVelocity()
{
	FRotator Rot = Base->GetComponentRotation();
	FVector Vel = Rot.UnrotateVector(Base->GetPhysicsLinearVelocity());
	return Vel.X;
}

// Gets the TransversalVelocity of the Base. 
const float UMWControllerComponent::GetBaseTransversalVelocity()
{
	FRotator Rot = Base->GetComponentRotation();
	FVector Vel = Rot.UnrotateVector(Base->GetPhysicsLinearVelocity());
	return Vel.Y;
}

// Gets the AngularVelocity of the Base. 
const float UMWControllerComponent::GetBaseAngularVelocity()
{
	FRotator Rot = Base->GetComponentRotation();
	FVector Vel = Rot.UnrotateVector(Base->GetPhysicsAngularVelocityInRadians());
	return Vel.Z;
}
