// Copyright 2018, Institute for Artificial Intelligence - University of Bremen
// Author: Patrick Kellmann

#include "MWControllerBaseHandler.h"

// Constructor
MWControllerBaseHandler::MWControllerBaseHandler(UMWControllerComponent* MWControllerComponent) :MWControllerHandler(MWControllerComponent)
{
}

// Destructor
MWControllerBaseHandler::~MWControllerBaseHandler()
{
}

// Sets the linear velocity of the base.
bool MWControllerBaseHandler::SetBaseLinearVelocity(const FVector Velocity)
{
	if (!CheckBase()) { return false; }

	FVector CalcedVector = CalcLongitudinal(Velocity) + CalcTransversal(Velocity);

	// meter to cm 
	CalcedVector.X *= SCALE_FACTOR_M_TO_CM;
	CalcedVector.Y *= SCALE_FACTOR_M_TO_CM;

	// Sets the velocity. Function needs physic, thus the base component must have activated physics.
	MWConComp->Base->SetAllPhysicsLinearVelocity(CalcedVector, false);
	MWConComp->Base->ComponentVelocity.Set(CalcedVector.X, CalcedVector.Y, CalcedVector.Z);

	return true;
}

// Calculates the velocity value based on the orientation.
FVector MWControllerBaseHandler::CalcLongitudinal(const FVector LongitudinalVelocity)
{
	const FRotator Rotation = MWConComp->Base->GetComponentRotation();
	const FVector Direction = FRotationMatrix(Rotation).GetScaledAxis(EAxis::X);

	return FVector(Direction * LongitudinalVelocity.X);
}

// Calculates the velocity value based on the orientation.
FVector MWControllerBaseHandler::CalcTransversal(const FVector TransversalVelocity)
{
	const FRotator Rotation = MWConComp->Base->GetComponentRotation();
	const FVector Direction = FRotationMatrix(Rotation).GetScaledAxis(EAxis::Y);

	return FVector(Direction * TransversalVelocity.Y);
}

// Sets the angular velocity of the base based on the orientation.
bool MWControllerBaseHandler::SetBaseAngularVelocity(const FVector AngularVelocity)
{
	if (!CheckBase()) { return false; }

	// X_Type can not rotate by definition.
	if (MWConComp->MWType == EMWType::MW_X_Type || !CheckBase())
	{
		MWConComp->Base->SetAllPhysicsAngularVelocityInRadians(FVector::ZeroVector, false);
		return false;
	}
	const FRotator Rotation = MWConComp->Base->GetComponentRotation();
	const FVector Direction = FRotationMatrix(Rotation).GetScaledAxis(EAxis::Z);
	const FVector CalcedVector = Direction * AngularVelocity.Z;

	MWConComp->Base->SetAllPhysicsAngularVelocityInRadians(CalcedVector, false);
	MWConComp->Base->ComponentVelocity.Set(CalcedVector.X, CalcedVector.Y, CalcedVector.Z);

	return true;
}
