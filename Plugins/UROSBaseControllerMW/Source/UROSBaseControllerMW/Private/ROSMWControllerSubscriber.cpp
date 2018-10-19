// Copyright 2018, Institute for Artificial Intelligence - University of Bremen
// Author: Patrick Kellmann

#include "ROSMWControllerSubscriber.h"
#include "std_msgs/String.h"

// Sets default values
AROSMWControllerSubscriber::AROSMWControllerSubscriber()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	IPAddress = TEXT("127.0.0.1");

	// Set Port to 9090
	Port = 9090;

	// /base/cmd_vel is standard for the Mecanum Wheel movement. 
	Topic = TEXT("/base/cmd_vel");

	MWConComp = nullptr;
}

// Called when the game starts or when spawned
void AROSMWControllerSubscriber::BeginPlay()
{
	Super::BeginPlay();

	// Set websocket server address to default
	Handler = MakeShareable<FROSBridgeHandler>(new FROSBridgeHandler(IPAddress, Port));

	// Connect to rosbridge
	Handler->Connect();

	if (this && this->IsValidLowLevel())
	{
		// Get the actor who holds the robot.
		TArray<AActor*> HelperList;
		UGameplayStatics::GetAllActorsOfClass(this->GetWorld(), AActor::StaticClass(), HelperList);
		for (auto RobAct : HelperList)
		{
			// Look for the default names. "Contains" is used because Unreal changes the name in some cases.
			if (RobAct->GetName().Contains(MWRobotBaseActorName))
			{
				if (RobAct && RobAct->IsValidLowLevel())
				{
					TArray<UMWControllerComponent*> ActorComponentList;
					RobAct->GetComponents<UMWControllerComponent>(ActorComponentList);

					for (auto ActComp : ActorComponentList)
					{
						//Search for the controller per tag. 
						if (ActComp->ComponentHasTag("MWControllerComponentTag"))
						{
							MWConComp = ActComp;

							// MWController must be present at BeginPlay.
							if (MWConComp && MWConComp->IsValidLowLevel())
							{

								// Create subscriber with callback class
								MWSubscriber = MakeShareable<UROSMWControllerSubscriberCallback>(
									new UROSMWControllerSubscriberCallback(Topic, TEXT("geometry_msgs/TwistStamped"), MWConComp));

								// Add subscriber to ROS handler
								Handler->AddSubscriber(MWSubscriber);
							}
							else
							{
								UE_LOG(LogTemp, Log, 
									TEXT("[%s] MWControllerComponent not found. No messages will be subscribed. %s"), 
									*FString(__FUNCTION__));
							}

							break;
						}
					}
				}
			}
		}
	}
}

// Called when the game starts or when spawned
void AROSMWControllerSubscriber::EndPlay(const EEndPlayReason::Type Reason)
{
	// Disconnect the handler
	Handler->Disconnect();
	Super::EndPlay(Reason);
}

// Called every frame
void AROSMWControllerSubscriber::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Tick handler
	Handler->Process();
}

