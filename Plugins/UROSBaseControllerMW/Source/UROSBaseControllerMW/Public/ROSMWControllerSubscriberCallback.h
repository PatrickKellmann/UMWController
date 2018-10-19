// Copyright 2018, Institute for Artificial Intelligence - University of Bremen
// Author: Patrick Kellmann

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ROSBridgeHandler.h"
#include "ROSBridgeSubscriber.h"
#include "MWControllerComponent.h"
#include "std_msgs/String.h"
#include "geometry_msgs/TwistStamped.h"

/**
 * This class serves as a callback to UROSBaseConMWSubscriber and forwards the ROS messages to the MWController.
 */
class UROSBASECONTROLLERMW_API UROSMWControllerSubscriberCallback : public FROSBridgeSubscriber
{
public:

	/*
	* Constructor des Callback.
	*
	* @param InTopic Topic of the message.
	* @param InType Type of the message.
	* @param MWCC MWControllerComponent to be addressed.
	*/
	UROSMWControllerSubscriberCallback(const FString& InTopic, const FString& InType, UMWControllerComponent* MWCC);

	/*
	* Destructor of the Callback.
	*/
	~UROSMWControllerSubscriberCallback();

	/*
	* Converts messages.
	*
	* @param JsonObject Messageobject.
	* @return parsed geometry_msgs::TwistStamped message.
	*/
	TSharedPtr<FROSBridgeMsg> ParseMessage(TSharedPtr<FJsonObject> JsonObject) const override;

	/*
	* Processes the messages and passes on specific data.
	*
	* @param Msg Message sent to UMWControllerComponent.
	*/
	void Callback(TSharedPtr<FROSBridgeMsg> Msg) override;

private:

	// The MWController to which the messages are sent.
	UMWControllerComponent* MWConComp = nullptr;
};
