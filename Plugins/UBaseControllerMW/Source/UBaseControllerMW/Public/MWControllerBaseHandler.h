// Copyright 2018, Institute for Artificial Intelligence - University of Bremen
// Author: Patrick Kellmann

#pragma once

#include "CoreMinimal.h"
#include "MWControllerHandler.h"

/**
 * This class handles all functions of the base.
 */
class UBASECONTROLLERMW_API MWControllerBaseHandler : private MWControllerHandler
{
public:

	/*
	* Constructor of the handler.
	*
	* @ param MWControllerComponent Owner of the handler.
	*/
	MWControllerBaseHandler(UMWControllerComponent* MWControllerComponent);

	/*
	* Destructor.
	*/
	~MWControllerBaseHandler();

	/*
	* Sets the transversal and longitudinal linear velocities as physic velocities.
	*
	* @ param DiagonalVelocity Velocity to be set.
	*/
	bool SetBaseLinearVelocity(const FVector Velocity);

	/*
	* Function sets the Angular Velocity of the RobotBase. This is only possible for O_Type configurations.
	*
	* @ param AngularVelocity Velocity to be set.
	*/
	bool SetBaseAngularVelocity(const FVector AngularVelocity);

private:
	/*
	* Function calculates the linear velocity for longitudinal (forward, backward) driving.
	*
	* @ param LongitudinalVelocity Velocity to be set.
	*/
	FVector CalcLongitudinal(const FVector LongitudinalVelocity);

	/*
	* Function calculates the linear velocity for transverse (lateral) driving.
	*
	* @ param TransversalVelocity Velocity to be set.
	*/
	FVector CalcTransversal(const FVector TransversalVelocity);
};
