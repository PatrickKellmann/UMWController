// Copyright 2018, Institute for Artificial Intelligence - University of Bremen
// Author: Patrick Kellmann

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "MWControllerDemoPawn.generated.h"

/*
* Pawn for camera control in demo mode.
*/
UCLASS()
class UBASECONTROLLERMWDEMO_API AMWControllerDemoPawn : public APawn
{
	GENERATED_BODY()

public:

	/*
	* Sets default values for this pawn's properties. Creates camera und stick.
	*/
	AMWControllerDemoPawn();

protected:
	
	/*
	* Called when the game starts or when spawned
	*/
	virtual void BeginPlay() override;

public:

	/*
	* Called every frame
	*/
	virtual void Tick(float DeltaTime) override;

	/*
	* Called to bind functionality to input.
	*/
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


	// Demo CameraBoom.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class USpringArmComponent* MWDemoCameraBoom;

	// Camera is tracking the MWRobotBaseActor.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class UCameraComponent* MWDemoFollowCamera;

	// Standard name for the actor MWRobotBaseActor.
	UPROPERTY(VisibleAnywhere)
		FString MWRobotBaseActorName = "MWRobotBaseActor";

	// Saves the actor holding the robot.
	AActor* MWRobotBaseActor = nullptr;

};
