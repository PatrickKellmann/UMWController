// Copyright 2019, Institute for Artificial Intelligence - University of Bremen
// Author: Patrick Kellmann

#pragma once

#include "CoreMinimal.h"
#include "MWControllerHandler.h"

/**
 *
 */
class UBASECONTROLLERMW_API MWControllerConstraintHandler : private MWControllerHandler
{
public:

	/**
	* Constructor of the handler.
	* @ param MWControllerComponent Owner of the handler.
	*/
	MWControllerConstraintHandler(UMWControllerComponent* MWControllerComponent);

	/**
	* Destructor.
	*/
	~MWControllerConstraintHandler();

	/**
	* Sets the constraints and deletes the list.
	 */
	bool SetupPhysicsConstraints();

	/**
	* Deletes the constraints.
	*/
	void DeletePhysicsConstraints();

private:

	/**
	* Sets the constraint.
	*
	* @ param PhyConsComp Constraint to be set.
	* @ param ConstraintActor1 Actor one of the constraint.
	* @ param ConstraintActor2 Actor two of the constraint.
	* @ param MeshComp Mesh of the constraint.
	*/
	void CreatePhysicsConstraint(UPhysicsConstraintComponent* PhyConsComp, AActor* ConstraintActor1, AActor* ConstraintActor2, UStaticMeshComponent* MeshComp);

};
