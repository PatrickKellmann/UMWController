// Copyright 2018, Institute for Artificial Intelligence - University of Bremen
// Author: Patrick Kellmann

#pragma once

#include "CoreMinimal.h"
#include "MWControllerHandler.h"

/**
 * This class describes the functions for the collisons and the BoxComponents (box collisons in the editor).
 */
class UBASECONTROLLERMW_API MWControllerBoxComponentHandler: private MWControllerHandler
{
	public:

	
	/*
	* Sets default values for this component's properties. 
	*/
	MWControllerBoxComponentHandler(UMWControllerComponent* MWControllerComponent);

	/*
	*
	*/
	~MWControllerBoxComponentHandler();

	/*
	* Creates new BoxComponents and configures them.
	*/
	bool CheckAndReleaseBoxComponents();

	/*
	* Configures the BoxComponents.
	*
	* @param BoxComp Component that is being edited.
	* @param MeshComp Mesh protected by the BoxComponent.
	* @param ColMat	Material that sets the friction to 0.
	*/
	bool AdjustBoxComponentForCollision(UBoxComponent* BoxComp, UStaticMeshComponent* MeshComp, UPhysicalMaterial* ColMat);

	/*
	* Destroys the BoxComponents currently in use. Also deletes old references.
	*/
	void DestroyBoxComponents();

};

