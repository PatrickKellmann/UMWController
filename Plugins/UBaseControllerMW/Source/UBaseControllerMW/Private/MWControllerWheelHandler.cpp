// Copyright 2018, Institute for Artificial Intelligence - University of Bremen
// Author: Patrick Kellmann

#include "MWControllerWheelHandler.h"

 // Constructor.
MWControllerWheelHandler::MWControllerWheelHandler(UMWControllerComponent* MWControllerComponent) :MWControllerHandler(MWControllerComponent)
{
}

// Destructor.
MWControllerWheelHandler::~MWControllerWheelHandler()
{
}

// Determines the distance between the wheels. 
bool MWControllerWheelHandler::IsDistanceBetweenWheelsValid()
{
	if (CheckAllWheels())
	{
		const FVector WheelLeftReraPostion = MWConComp->WheelLeftRear->RelativeLocation;
		const FVector WheelLeftFrontPostion = MWConComp->WheelLeftFront->RelativeLocation;
		const FVector WheelRightReraPostion = MWConComp->WheelRightRear->RelativeLocation;
		const FVector WheelRightFrontPostion = MWConComp->WheelRightFront->RelativeLocation;

		const float DistanceBetweenFrontWheels = FVector::Distance(WheelLeftFrontPostion, WheelRightFrontPostion);
		const float DistanceBetweenReraWheels = FVector::Distance(WheelLeftReraPostion, WheelRightReraPostion);

		// Deviations are tolerated to some degree. 
		return FMath::IsNearlyEqual(DistanceBetweenFrontWheels, DistanceBetweenReraWheels, DEVIATION_VALUE) ? true : false;
	}
	return false;
}

// Determines the (half) distance between the wheels.  
TPair<float, float> MWControllerWheelHandler::CalcDistanceBetweenWheels()
{
	TPair<float, float> WheelDistance(0, 0);

	if (CheckAllWheels())
	{
		const FVector WheelLeftReraPostion = MWConComp->WheelLeftRear->RelativeLocation;
		const FVector WheelLeftFrontPostion = MWConComp->WheelLeftFront->RelativeLocation;
		const FVector WheelRightReraPostion = MWConComp->WheelRightRear->RelativeLocation;
		const FVector WheelRightFrontPostion = MWConComp->WheelRightFront->RelativeLocation;

		const float DistanceBetweenFrontWheels = FVector::Distance(WheelLeftFrontPostion, WheelRightFrontPostion);

		// Calculates half of the distance between the front wheels.
		WheelDistance.Key = DistanceBetweenFrontWheels / HALF_DISTANCE_DIVIDER;

		// The distance may differ slightly, as they do not stay the same during playtime.
		// The half of the value because the mathematical formulas work with these values.
		WheelDistance.Value = (FVector::Distance(WheelLeftFrontPostion, WheelLeftReraPostion)) / HALF_DISTANCE_DIVIDER;
	}
	return WheelDistance;
}

// Calculates the wheel size in diameter. Seeks the right value.
// All wheels must have the same orientation.
float MWControllerWheelHandler::CalcWheelDiameterInCentimeter()
{
	if (CheckAllWheels())
	{
		// Scale is ignored because the StaticMeshes are used for the calculation to get proper bounding boxes.
		const float DiameterWLFX = MWConComp->WheelLeftFront->GetStaticMesh()->GetBoundingBox().GetExtent().X * RADIUS_TO_DIAMETER;
		const float DiameterWRFX = MWConComp->WheelRightFront->GetStaticMesh()->GetBoundingBox().GetExtent().X * RADIUS_TO_DIAMETER;
		const float DiameterWLRX = MWConComp->WheelLeftRear->GetStaticMesh()->GetBoundingBox().GetExtent().X * RADIUS_TO_DIAMETER;
		const float DiameterWRRX = MWConComp->WheelRightRear->GetStaticMesh()->GetBoundingBox().GetExtent().X * RADIUS_TO_DIAMETER;

		const float DiameterWLFY = MWConComp->WheelLeftFront->GetStaticMesh()->GetBoundingBox().GetExtent().Y * RADIUS_TO_DIAMETER;
		const float DiameterWRFY = MWConComp->WheelRightFront->GetStaticMesh()->GetBoundingBox().GetExtent().Y * RADIUS_TO_DIAMETER;
		const float DiameterWLRY = MWConComp->WheelLeftRear->GetStaticMesh()->GetBoundingBox().GetExtent().Y * RADIUS_TO_DIAMETER;
		const float DiameterWRRY = MWConComp->WheelRightRear->GetStaticMesh()->GetBoundingBox().GetExtent().Y * RADIUS_TO_DIAMETER;

		const float DiameterWLFZ = MWConComp->WheelLeftFront->GetStaticMesh()->GetBoundingBox().GetExtent().Z * RADIUS_TO_DIAMETER;
		const float DiameterWRFZ = MWConComp->WheelRightFront->GetStaticMesh()->GetBoundingBox().GetExtent().Z * RADIUS_TO_DIAMETER;
		const float DiameterWLRZ = MWConComp->WheelLeftRear->GetStaticMesh()->GetBoundingBox().GetExtent().Z * RADIUS_TO_DIAMETER;
		const float DiameterWRRZ = MWConComp->WheelRightRear->GetStaticMesh()->GetBoundingBox().GetExtent().Z * RADIUS_TO_DIAMETER;

		const float DiameterX = (DiameterWLFX + DiameterWRFX + DiameterWLRX + DiameterWRRX) / WHEEL_NUMBER_DIVIDER;
		const float DiameterY = (DiameterWLFY + DiameterWRFY + DiameterWLRY + DiameterWRRY) / WHEEL_NUMBER_DIVIDER;
		const float DiameterZ = (DiameterWLFZ + DiameterWRFZ + DiameterWLRZ + DiameterWRRZ) / WHEEL_NUMBER_DIVIDER;

		// Gives DiameterX or DiameterZ, as a wheel on two levels is a circle with a constant distance to the center.
		return FMath::IsNearlyEqual(DiameterX, DiameterY, DEVIATION_VALUE) ? DiameterX : DiameterZ;
	}
	return -1.f;
}

// Starts the wheel calculations and rotation position.
void MWControllerWheelHandler::SetupWheelsMovement(const FVector Velocity)
{
	if (MWConComp && CheckAllWheels() && MWConComp->WheelList.Num() == 4)
	{
		// For the angular velocity of the wheels.
		CalcWheelsAngularVelocity(Velocity);
		RotateWheels();

		//TODO
		// For the linear velocity of the wheels (same as base-velocity).
		//SetWheelsMovementVelocities(Velocity);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[%s][%d]. There was a problem."), TEXT(__FUNCTION__), __LINE__);
	}
}

// This function calculates the turning of the wheels (visual component).
void MWControllerWheelHandler::RotateWheels()
{
	// The wheels can only rotate freely in one axis.
	const FRotator Rotation = MWConComp->WheelLeftFront->GetComponentRotation();
	const FVector Direction = FRotationMatrix(Rotation).GetScaledAxis(EAxis::Y);

	MWConComp->WheelLeftFront->SetAllPhysicsAngularVelocityInRadians(Direction * MWConComp->WheelLeftFrontAngularVelocity);
	MWConComp->WheelRightFront->SetAllPhysicsAngularVelocityInRadians(Direction * MWConComp->WheelRightFrontAngularVelocity);
	MWConComp->WheelLeftRear->SetAllPhysicsAngularVelocityInRadians(Direction * MWConComp->WheelLeftRearAngularVelocity);
	MWConComp->WheelRightRear->SetAllPhysicsAngularVelocityInRadians(Direction * MWConComp->WheelRightRearAngularVelocity);
}

// Calculates the angular wheel velocity. 
void MWControllerWheelHandler::CalcWheelsAngularVelocity(const FVector Velocity)
{
	// The formula works with meters.
	float WheelRadius = MWConComp->WheelDiameterInCentimeter / DIAMETER_TO_RADIUS;
	float CombinedDis = MWConComp->CombinedDistanceValue / SCALE_FACTOR_CM_TO_M;

	// The wheels must be oriented the same way. In addition, they must be turned correctly (drive forward, turn forward, etc.). 
	// Therefore, it may happen that the anular velocity of the wheels requires a polarity (adjustable in the editor).
	if (MWConComp->MWType == EMWType::MW_O_Type)
	{
		MWConComp->WheelLeftFrontAngularVelocity = 1.f / WheelRadius *
			((Velocity.X + Velocity.Y - (CombinedDis * Velocity.Z * MWConComp->PolarityForAngularMovement))
				* SCALE_FACTOR_M_TO_CM);
		MWConComp->WheelLeftRearAngularVelocity = 1.f / WheelRadius *
			((Velocity.X - Velocity.Y - (CombinedDis * Velocity.Z * MWConComp->PolarityForAngularMovement))
				* SCALE_FACTOR_M_TO_CM);
		MWConComp->WheelRightFrontAngularVelocity = 1.f / WheelRadius *
			((Velocity.X - Velocity.Y + (CombinedDis * Velocity.Z * MWConComp->PolarityForAngularMovement))
				* SCALE_FACTOR_M_TO_CM);
		MWConComp->WheelRightRearAngularVelocity = 1.f / WheelRadius *
			((Velocity.X + Velocity.Y + (CombinedDis * Velocity.Z * MWConComp->PolarityForAngularMovement))
				* SCALE_FACTOR_M_TO_CM);
	}
	else
	{
		// X_Type has no angular movement so the formular does not need that part. 
		MWConComp->WheelLeftFrontAngularVelocity = 1.f / WheelRadius * ((Velocity.X - Velocity.Y)
			* SCALE_FACTOR_M_TO_CM);
		MWConComp->WheelLeftRearAngularVelocity = 1.f / WheelRadius * ((Velocity.X + Velocity.Y)
			* SCALE_FACTOR_M_TO_CM);
		MWConComp->WheelRightFrontAngularVelocity = 1.f / WheelRadius * ((Velocity.X + Velocity.Y)
			* SCALE_FACTOR_M_TO_CM);
		MWConComp->WheelRightRearAngularVelocity = 1.f / WheelRadius * ((Velocity.X - Velocity.Y)
			* SCALE_FACTOR_M_TO_CM);
	}
}


// Sets the linear speeds of the wheels.
void MWControllerWheelHandler::SetWheelsMovementVelocities(const FVector Velocity)
{
	for (UStaticMeshComponent* Wheel : MWConComp->WheelList)
	{
		FVector CalcedVector = CalcLongitudinal(Velocity, Wheel) + CalcTransversal(Velocity, Wheel);
		CalcedVector.X *= SCALE_FACTOR_M_TO_CM;
		CalcedVector.Y *= SCALE_FACTOR_M_TO_CM;


		if (!FMath::IsNearlyEqual(Velocity.Z, 0.f))
		{

			//TODO
			FVector CalcedCrossVector = CalcAngular(Velocity, Wheel);
			CalcedVector += CalcedCrossVector;

			ShiftWheelsForAngularMovement(Velocity, Wheel);

		}


		Wheel->SetAllPhysicsLinearVelocity(CalcedVector, false);
		Wheel->ComponentVelocity.Set(CalcedVector.X, CalcedVector.Y, CalcedVector.Z);
	}
}


// Calculates the velocity value based on the orientation.
FVector MWControllerWheelHandler::CalcLongitudinal(const FVector LongitudinalVelocity, const UStaticMeshComponent* Wheel)
{
	const FRotator Rotation = Wheel->GetComponentRotation();
	const FVector Direction = FRotationMatrix(Rotation).GetScaledAxis(EAxis::X);

	return FVector(Direction * LongitudinalVelocity.X);
}

// Calculates the velocity value based on the orientation.
FVector MWControllerWheelHandler::CalcTransversal(const FVector TransversalVelocity, const UStaticMeshComponent* Wheel)
{
	const FRotator Rotation = Wheel->GetComponentRotation();
	const FVector Direction = FRotationMatrix(Rotation).GetScaledAxis(EAxis::Y);

	return FVector(Direction * TransversalVelocity.Y);
}

// Calculates the velocity value based on the orientation.
FVector MWControllerWheelHandler::CalcAngular(const FVector AngularVelocity, const UStaticMeshComponent* Wheel)
{
	// TODO 
	FVector WheelCenter = Wheel->GetCenterOfMass();
	//FVector BaseCenter = MWConComp->Base->GetCenterOfMass();
	//float RadiusDis = FVector::Distance(WheelCenter, BaseCenter);
	// WheelCenter = RadiusVector

	return FVector::CrossProduct(FVector(AngularVelocity), WheelCenter);
}

// TODO 
void MWControllerWheelHandler::ShiftWheelsForAngularMovement(const FVector AngularVelocity, UStaticMeshComponent* Wheel)
{
	// X_Type can not rotate by definition.
	if (MWConComp->MWType == EMWType::MW_X_Type || !CheckBase())
	{
		MWConComp->Base->SetAllPhysicsAngularVelocityInRadians(FVector::ZeroVector, false);
		return;
	}
	const FRotator Rotation = MWConComp->Base->GetComponentRotation();
	const FVector Direction = FRotationMatrix(Rotation).GetScaledAxis(EAxis::Z);
	const FVector CalcedVector = Direction * AngularVelocity.Z;

	//const FRotator Rotation = Wheel->GetComponentRotation();
	//const FVector Direction = FRotationMatrix(Rotation).GetScaledAxis(EAxis::Z);
	//const FVector CalcedVector = Direction * AngularVelocity.Z;

	Wheel->SetAllPhysicsAngularVelocityInRadians(CalcedVector);
	Wheel->ComponentVelocity.Set(CalcedVector.X, CalcedVector.Y, CalcedVector.Z);
}