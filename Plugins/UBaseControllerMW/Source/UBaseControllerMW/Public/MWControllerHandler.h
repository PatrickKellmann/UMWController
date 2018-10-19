// Copyright 2018, Institute for Artificial Intelligence - University of Bremen
// Author: Patrick Kellmann

#pragma once

#include "CoreMinimal.h"
#include "MWControllerComponent.h"
#include "Components/StaticMeshComponent.h"

class UMWControllerComponent;

/**
 * This class acts as a parent for the handlers and defines some aspects that all handlers share.
 */
class UBASECONTROLLERMW_API MWControllerHandler
{

protected:

	/*
	* Constructor of the handler. Assigns MWConComp to the creating MWController.
	*
	* @param MWControllerComponent Owner of the handler. 
	*/
	MWControllerHandler(UMWControllerComponent* MWControllerComponent);

	/*
	* Destructor of the handler. 
	*/
	virtual ~MWControllerHandler();

	/*
	* Checks if all wheels are present.
	*
	* @return true if all wheels are present else false. 
	*/
	bool CheckAllWheels();

	/*
	* Checks if the base is present.
	*
	* @preturn true if the base are present else false. 
	*/
	bool CheckBase();

	// The controller that acts as an interface to Unreal.
	UMWControllerComponent* MWConComp;
};
