// Copyright 2018, Institute for Artificial Intelligence - University of Bremen
// Author: Patrick Kellmann

#pragma once

#include "CoreMinimal.h"
#include "ReflexxesAPI.h"
#include "RMLVelocityFlags.h"
#include "RMLVelocityInputParameters.h"
#include "RMLVelocityOutputParameters.h"


#define CYCLE_TIME_IN_SECONDS            0.001
#define NUMBER_OF_DOFS					 3

/**
 * Defines a class that uses Reflexxes to interpolate in Twist-Space
 */
class UBASECONTROLLERMW_API MWControllerInterpolator
{

public:

	/*
	* Constructor of the interpolator.
	*
	* @param To change the speed of the intermediate values. Should match the game speed.
	*/
	MWControllerInterpolator(const float CycTimInSec);

	/*
	* Destructor of the interpolator.
	*/
	~MWControllerInterpolator();

	/*
	* Sets the current transform of the robot.
	*
	* @param X X specification of the location.
	* @param Y Y specification of the location.
	* @param Theta Theta Specification of the orientation.
	*/
	void set_current_pose(double X, double Y, double Theta);

	/*
	* Sets the desired twist.
	*
	* @param DX X specification of the twist.
	* @param DY Y specification of the twist
	* @param DTheta Theta Specification of the twist
	*/
	void set_target_twist(double DX, double DY, double Dtheta);

	/*
	* Gets the next twist.
	*
	* @return FVector with new twist values.
	*/
	FVector get_next_twist();


private:

	int32 result;
	ReflexxesAPI *RML;
	RMLVelocityInputParameters *IP;
	RMLVelocityOutputParameters *OP;
	RMLVelocityFlags Flags;
};
