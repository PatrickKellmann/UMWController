// Copyright 2018, Institute for Artificial Intelligence - University of Bremen
// Author: Patrick Kellmann

#include "MWControllerInterpolator.h"

// Constructor.
MWControllerInterpolator::MWControllerInterpolator(const float CycTimInSec) : RML(NULL), IP(NULL), OP(NULL)
{

	RML = new ReflexxesAPI(NUMBER_OF_DOFS, CycTimInSec);
	IP = new RMLVelocityInputParameters(NUMBER_OF_DOFS);
	OP = new RMLVelocityOutputParameters(NUMBER_OF_DOFS);

	IP->CurrentPositionVector->VecData[0] = 0.0;
	IP->CurrentPositionVector->VecData[1] = 0.0;
	IP->CurrentPositionVector->VecData[2] = 0.0;

	IP->CurrentVelocityVector->VecData[0] = 0.0;
	IP->CurrentVelocityVector->VecData[1] = 0.0;
	IP->CurrentVelocityVector->VecData[2] = 0.0;

	IP->CurrentAccelerationVector->VecData[0] = 0.0;
	IP->CurrentAccelerationVector->VecData[1] = 0.0;
	IP->CurrentAccelerationVector->VecData[2] = 0.0;

	IP->MaxAccelerationVector->VecData[0] = 2.0;
	IP->MaxAccelerationVector->VecData[1] = 2.0;
	IP->MaxAccelerationVector->VecData[2] = 2.0;

	IP->MaxJerkVector->VecData[0] = 10.0;
	IP->MaxJerkVector->VecData[1] = 10.0;
	IP->MaxJerkVector->VecData[2] = 10.0;

	IP->TargetVelocityVector->VecData[0] = 0.0;
	IP->TargetVelocityVector->VecData[1] = 0.0;
	IP->TargetVelocityVector->VecData[2] = 0.0;

	IP->SelectionVector->VecData[0] = true;
	IP->SelectionVector->VecData[1] = true;
	IP->SelectionVector->VecData[2] = true;
}

// Destructor.
MWControllerInterpolator::~MWControllerInterpolator()
{
	delete RML;
	RML = nullptr;

	delete IP;
	IP = nullptr;

	delete OP;
	OP = nullptr;
}

// Sets the current transform.
void MWControllerInterpolator::set_current_pose(double X, double Y, double Theta) 
{
	if (IP && RML && OP)
	{
		IP->CurrentPositionVector->VecData[0] = X;
		IP->CurrentPositionVector->VecData[1] = Y;
		IP->CurrentPositionVector->VecData[2] = Theta;
	}
}

// Sets the desired twist.
void MWControllerInterpolator::set_target_twist(double DX, double DY, double Dtheta) 
{
	if (IP && RML && OP)
	{
		IP->TargetVelocityVector->VecData[0] = DX;
		IP->TargetVelocityVector->VecData[1] = DY;
		IP->TargetVelocityVector->VecData[2] = Dtheta;
	}
}

// Gets the next twist. 
FVector  MWControllerInterpolator::get_next_twist() 
{
	if (IP && RML && OP)
	{
		result = RML->RMLVelocity(*IP, OP, Flags);

		if (result < 0)
		{
			UE_LOG(LogTemp, Error, TEXT("[%s][%d]. An error occurred (%d)."), TEXT(__FUNCTION__), __LINE__, result);
			return FVector::ZeroVector;
		}

		// Feeds the output values of the current control cycle back to
		// input values of the next control cycle

		*IP->CurrentPositionVector = *OP->NewPositionVector;
		*IP->CurrentVelocityVector = *OP->NewVelocityVector;
		*IP->CurrentAccelerationVector = *OP->NewAccelerationVector;

		double &DX = OP->NewVelocityVector->VecData[0];
		double &DY = OP->NewVelocityVector->VecData[1];
		double &Dtheta = OP->NewVelocityVector->VecData[2];

		return FVector(float(DX), float(DY), float(Dtheta));
	}
	return FVector::ZeroVector;
}
