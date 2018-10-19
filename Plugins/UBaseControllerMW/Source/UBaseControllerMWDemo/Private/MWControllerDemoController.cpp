// Copyright 2018, Institute for Artificial Intelligence - University of Bremen
// Author: Patrick Kellmann

#include "MWControllerDemoController.h"
#include "Kismet/GameplayStatics.h"

// Constructor. 
AMWControllerDemoController::AMWControllerDemoController()
{
}

// Prepares the controller for work.
void AMWControllerDemoController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// Binding the control commands.
	InputComponent->BindAxis("LongitudinalMovement", this, &AMWControllerDemoController::SetLongitudinalMovement);
	InputComponent->BindAxis("TransversalMovement", this, &AMWControllerDemoController::SetTransversalMovement);
	InputComponent->BindAxis("AngularMovement", this, &AMWControllerDemoController::SetAngularMovement);

	// Deadman switch.
	InputComponent->BindAction("Deadman", IE_Pressed, this, &AMWControllerDemoController::DeadmanPressed);
	InputComponent->BindAction("Deadman", IE_Released, this, &AMWControllerDemoController::DeadmanReleased);
}

// Searches at BeginPlay the MWController of the founded Actor.
void AMWControllerDemoController::BeginPlay()
{
	if (this && this->IsValidLowLevel())
	{
		// Get the Actor who holds the robot.
		TArray<AActor*> HelperList;
		UGameplayStatics::GetAllActorsOfClass(this->GetWorld(), AActor::StaticClass(), HelperList);

		for (auto RobAct : HelperList)
		{
			// Looks for the default names. "Contains" is used because Unreal changes the name in some cases.
			if (RobAct->GetName().Contains(MWRobotBaseActorName))
			{
				MWRobotBaseActor = RobAct;
				break;
			}
		}
	}

	if (MWRobotBaseActor && MWRobotBaseActor->IsValidLowLevel())
	{
		TArray<UMWControllerComponent*> ActorComponentList;
		MWRobotBaseActor->GetComponents<UMWControllerComponent>(ActorComponentList);

		for (auto ActComp : ActorComponentList)
		{
			// The MWController is searched via the Tag.
			if (ActComp->ComponentHasTag("MWControllerComponentTag"))
			{
				MWConComp = ActComp;
				break;
			}
		}
		if (!MWConComp)
		{
			this->SetActorTickEnabled(false);
			UE_LOG(LogTemp, Error,
				TEXT("[%s][%d]. MWController was nullptr. Tick of MWControllerDemoController will be turn off."),
				TEXT(__FUNCTION__), __LINE__);
		}
	}
	else
	{
		this->SetActorTickEnabled(false);
		UE_LOG(LogTemp, Error,
			TEXT("[%s][%d]. MWRobotBaseActor was nullptr. Tick of MWControllerDemoController will be turn off."),
			TEXT(__FUNCTION__), __LINE__);
	}
}

// Called every frame
void AMWControllerDemoController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	// The inputs are collected and sent to the MWController by tick.
	// This corresponds to UROSBridge.
	FGraphEventRef Task = FFunctionGraphTask::CreateAndDispatchWhenReady([&, this]()
	{
		if (bDeadman) 
		{
			if (MWConComp && MWConComp->IsValidLowLevel() || !MWConComp->IsBeingDestroyed())
			{

				MWConComp->ReceiveROSMessage(LinearVelocity, AngularVelocity);
			}
			else 
			{
				this->SetActorTickEnabled(false);
				UE_LOG(LogTemp, Error,
					TEXT("[%s][%d]. MWController was nullptr. Tick of AMWControllerDemoController will be turn off."),
					TEXT(__FUNCTION__), __LINE__);
			}
		}
	}, TStatId(), nullptr, ENamedThreads::GameThread);
}

// Command to move the robotbase Longitudinally (forward, backward).
void AMWControllerDemoController::SetLongitudinalMovement(const float ControllerInput)
{

	if ((ControllerInput != 0.0f) && MWConComp && MWRobotBaseActor)
	{
		if (bDeadman) 
		{
			LinearVelocity.X = ControllerInput;
		}
	}
	else
	{
		LinearVelocity.X = 0.f;
	}
}

// Command to move the robotbase transversal (left, right).
void AMWControllerDemoController::SetTransversalMovement(const float ControllerInput)
{
	if ((ControllerInput != 0.0f) && MWConComp && MWRobotBaseActor)
	{
		if (bDeadman) 
		{
			LinearVelocity.Y = ControllerInput;
		}
	}
	else
	{
		LinearVelocity.Y = 0.f;
	}
}

// Command to move the robotbase angular (Z = Yaw rotation).
void AMWControllerDemoController::SetAngularMovement(const float ControllerInput)
{
	if ((ControllerInput != 0.0f) && MWConComp && MWRobotBaseActor)
	{
		if (bDeadman)
		{
			AngularVelocity.Z = ControllerInput;
		}
	}
	else
	{
		AngularVelocity.Z = 0.f;
	}
}

//  Deadman's circuit is pressed.
void AMWControllerDemoController::DeadmanPressed()
{
	bDeadman = true;
}

//  Deadman's circuit is not pressed.
void AMWControllerDemoController::DeadmanReleased() 
{
	bDeadman = false;
}