// Copyright 2018, Institute for Artificial Intelligence - University of Bremen
// Author: Patrick Kellmann

#include "MWControllerHandler.h"

// Constructor.
MWControllerHandler::MWControllerHandler(UMWControllerComponent* MWControllerComponent)
{
	MWConComp = MWControllerComponent;
}

// Destructor.
MWControllerHandler::~MWControllerHandler()
{
	MWConComp = nullptr;
}

// Checks if all wheel elements are present.
bool MWControllerHandler::CheckAllWheels()
{
	return MWConComp->WheelLeftFront 
		&& MWConComp->WheelRightFront 
		&& MWConComp->WheelLeftRear
		&& MWConComp->WheelRightRear ? true : false;
}

//	Checks if the base is present.
bool MWControllerHandler::CheckBase()
{
	return MWConComp->Base ? true : false;
}
