// Copyright 2018, Institute for Artificial Intelligence - University of Bremen
// Author: Patrick Kellmann

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ROSBridgeHandler.h"	 
#include "MWControllerComponent.h"
#include "ROSMWControllerSubscriberCallback.h"
#include "ROSMWControllerSubscriber.generated.h"

/*
* Subscriber for MWController.
*/
UCLASS()
class UROSBASECONTROLLERMW_API AROSMWControllerSubscriber : public AActor
{
	GENERATED_BODY()

public:

	/*
	* Sets default values for this actor's properties.
	*/
	AROSMWControllerSubscriber();

protected:
	/*
	* Called when the game starts or when spawned. Search for the MWController via the actor.
	*/
	virtual void BeginPlay() override;

	/*
	* Called when game ends or actor deleted
	*
	* @param Reason reason for end play.
	*/
	virtual void EndPlay(const EEndPlayReason::Type Reason) override;

	/*
	*  Called every frame
	*
	* @param DeltaTime Holds current delta time in seconds.
	*/
	virtual void Tick(float DeltaTime) override;

public:

	// Address for communication.
	UPROPERTY(EditAnywhere, Category = "ROS Subscriber", meta = (ToolTip = "IP address of the ROS computer"))
		FString IPAddress;

	// Port for communication.
	UPROPERTY(EditAnywhere, Category = "ROS Subscriber", meta = (ToolTip = "Port to be used"))
		uint32 Port;

	// Topic of the message to be received.
	UPROPERTY(EditAnywhere, Category = "ROS Subscriber", meta = (ToolTip = "Topic that is used. Default setting is standard for MWController."))
		FString Topic;

private:
	// Add a smart pointer to ROSBridgeHandler
	TSharedPtr<FROSBridgeHandler> Handler;

	// Add a ROSBridgePublisher smart pointer
	TSharedPtr<UROSMWControllerSubscriberCallback> MWSubscriber;

	// MWController to which the messages should be routed.
	UMWControllerComponent* MWConComp;

	// Defaultname for the Actor MWRobotBaseActor. Must be changed in the editor if several MW controllers are to be addressed.
	UPROPERTY(EditAnywhere, Category = "MWRobotBaseActor", meta = (ToolTip = "Name of the robot Actor. Must be changed in the editor if several MWControllers are to be addressed."))
		FString MWRobotBaseActorName = "MWRobotBaseActor";
};
