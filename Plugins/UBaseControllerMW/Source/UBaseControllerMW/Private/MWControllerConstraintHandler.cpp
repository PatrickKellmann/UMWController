// Copyright 2019, Institute for Artificial Intelligence - University of Bremen
// Author: Patrick Kellmann

#include "MWControllerConstraintHandler.h"

 // Constructor.
MWControllerConstraintHandler::MWControllerConstraintHandler(UMWControllerComponent* MWControllerComponent) :MWControllerHandler(MWControllerComponent)
{
}

// Destructor.
MWControllerConstraintHandler::~MWControllerConstraintHandler()
{
	DeletePhysicsConstraints();
}

// Sets the constraints and deletes the list.
bool MWControllerConstraintHandler::SetupPhysicsConstraints()
{

	// There must be exactly four constraints.
	if (MWConComp->ConstraintList.Num() == 4)
	{
		for (auto ConsElem : MWConComp->ConstraintList)
		{
			CreatePhysicsConstraint(ConsElem.PhyCon, ConsElem.ActorOne, ConsElem.ActorTwo, ConsElem.MeshComp);
			ConsElem.Destroy();
		}

		MWConComp->ConstraintList.Empty();

		return true; 
	}
	return false; 
}


// Sets the constraint.
void MWControllerConstraintHandler::CreatePhysicsConstraint(UPhysicsConstraintComponent* PhyConsComp, AActor* ConstraintActor1, AActor* ConstraintActor2, UStaticMeshComponent* MeshComp)
{
	PhyConsComp->ConstraintActor1 = ConstraintActor1;
	PhyConsComp->ConstraintActor2 = ConstraintActor2;
	PhyConsComp->SetConstrainedComponents(MWConComp->Base, NAME_None, MeshComp, NAME_None);
	PhyConsComp->SetWorldTransform(MeshComp->GetComponentTransform());
	PhyConsComp->SetAngularSwing1Limit(EAngularConstraintMotion::ACM_Locked, 0.f);
	PhyConsComp->SetAngularSwing2Limit(EAngularConstraintMotion::ACM_Free, 0.f);
	PhyConsComp->SetAngularTwistLimit(EAngularConstraintMotion::ACM_Locked, 0.f);

	PhyConsComp->SetDisableCollision(true);
	PhyConsComp->ConstraintInstance.DisableProjection();

	PhyConsComp->bEditableWhenInherited = true;
}

// Deletes the constraints.
void MWControllerConstraintHandler::DeletePhysicsConstraints()
{
	if (MWConComp) {
		if (MWConComp->PhysicsConstBaseWheelLF)
		{
			MWConComp->PhysicsConstBaseWheelLF->DestroyComponent();
			MWConComp->PhysicsConstBaseWheelLF = nullptr;
		}
		if (MWConComp->PhysicsConstBaseWheelRF)
		{
			MWConComp->PhysicsConstBaseWheelRF->DestroyComponent();
			MWConComp->PhysicsConstBaseWheelRF = nullptr;
		}
		if (MWConComp->PhysicsConstBaseWheelLR)
		{
			MWConComp->PhysicsConstBaseWheelLR->DestroyComponent();
			MWConComp->PhysicsConstBaseWheelLR = nullptr;
		}
		if (MWConComp->PhysicsConstBaseWheelRR)
		{
			MWConComp->PhysicsConstBaseWheelRR->DestroyComponent();
			MWConComp->PhysicsConstBaseWheelRR = nullptr;
		}
	}
}
