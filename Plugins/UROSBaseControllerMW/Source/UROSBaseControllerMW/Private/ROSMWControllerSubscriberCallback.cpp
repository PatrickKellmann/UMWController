// Copyright 2018, Institute for Artificial Intelligence - University of Bremen
// Author: Patrick Kellmann

#include "ROSMWControllerSubscriberCallback.h"
#include "Conversions.h"

// Sets default values
UROSMWControllerSubscriberCallback::UROSMWControllerSubscriberCallback(
	const FString& InTopic, const FString& InType, UMWControllerComponent* MWCC) :
	FROSBridgeSubscriber(InTopic, InType)
{
	MWConComp = MWCC;
}

// Destructor of the class. 
UROSMWControllerSubscriberCallback::~UROSMWControllerSubscriberCallback()
{
}

// Parse Message 
TSharedPtr<FROSBridgeMsg> UROSMWControllerSubscriberCallback::ParseMessage
(TSharedPtr<FJsonObject> JsonObject) const
{
	TSharedPtr<geometry_msgs::TwistStamped> MWMessage =
		MakeShareable<geometry_msgs::TwistStamped>(new geometry_msgs::TwistStamped());

	MWMessage->FromJson(JsonObject);

	return StaticCastSharedPtr<FROSBridgeMsg>(MWMessage);
}

// Sends Messages. 
void UROSMWControllerSubscriberCallback::Callback(TSharedPtr<FROSBridgeMsg> Msg)
{
	// Defines message type.
	TSharedPtr<geometry_msgs::TwistStamped> MWMessage = StaticCastSharedPtr<geometry_msgs::TwistStamped>(Msg);

	// Gets the vectors out of TwistStamped.
	FVector LinearVelocity = MWMessage.Get()->GetTwist().GetLinear().GetVector();
	FVector AngularVelocity = MWMessage.Get()->GetTwist().GetAngular().GetVector();

	// Transformation for the correct system in Unreal
	AngularVelocity.Z *= -1;

	// To use the same thread as MWController. 
	FGraphEventRef Task = FFunctionGraphTask::CreateAndDispatchWhenReady([&, this, LinearVelocity, AngularVelocity]()
	{
		if (MWConComp && MWConComp->IsValidLowLevel() && !MWConComp->IsBeingDestroyed())
		{
			// Convert ROS right-hand to unreal left-hand. m/s is needed for most formulas and the interpolator, so it needs to be reverted. 
			MWConComp->ReceiveROSMessage(FConversions::CmToM(FConversions::ROSToU(LinearVelocity)), AngularVelocity);
		}
	}, TStatId(), nullptr, ENamedThreads::GameThread);
}