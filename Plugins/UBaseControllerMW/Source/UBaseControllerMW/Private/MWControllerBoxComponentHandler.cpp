// Copyright 2018, Institute for Artificial Intelligence - University of Bremen
// Author: Patrick Kellmann

#include "MWControllerBoxComponentHandler.h"

// Constructor.
MWControllerBoxComponentHandler::MWControllerBoxComponentHandler(UMWControllerComponent* MWControllerComponent) :MWControllerHandler(MWControllerComponent)
{
}

// Destructor.
MWControllerBoxComponentHandler::~MWControllerBoxComponentHandler()
{
}


// Creates the collision box around the wheels and around the base.
bool MWControllerBoxComponentHandler::CheckAndReleaseBoxComponents()
{

	if (CheckBase() && CheckAllWheels()) 
	{
		// Destroys the old (inherited) references that can not be deleted in the editor itself.
		for (auto Box : MWConComp->CreatedBoxComponentsList)
		{
			Box.Value->DestroyComponent();
		}

		// Created for the wheels and the base BoxComponents that protects the meshes from impacts.
		MWConComp->BoxComponentBase = NewObject<UBoxComponent>(MWConComp, FName(*("BoxComponent_" + MWConComp->BaseName)));

		MWConComp->BoxComponentLeftFront = NewObject<UBoxComponent>(MWConComp, FName(*("BoxComponent_" + MWConComp->WheelNameLeftFront)));
		MWConComp->BoxComponentRightFront = NewObject<UBoxComponent>(MWConComp, FName(*("BoxComponent_" + MWConComp->WheelNameRightFront)));
		MWConComp->BoxComponentLeftRear = NewObject<UBoxComponent>(MWConComp, FName(*("BoxComponent_" + MWConComp->WheelNameLeftRear)));
		MWConComp->BoxComponentRightRear = NewObject<UBoxComponent>(MWConComp, FName(*("BoxComponent_" + MWConComp->WheelNameRightRear)));

		UPhysicalMaterial* CollisionMaterial = NewObject<UPhysicalMaterial>(MWConComp, TEXT("CollisionMaterial"));
		CollisionMaterial->Friction = 0.f;

		AdjustBoxComponentForCollision(MWConComp->BoxComponentBase, MWConComp->Base, CollisionMaterial);
		AdjustBoxComponentForCollision(MWConComp->BoxComponentLeftFront, MWConComp->WheelLeftFront, CollisionMaterial);
		AdjustBoxComponentForCollision(MWConComp->BoxComponentRightFront, MWConComp->WheelRightFront, CollisionMaterial);
		AdjustBoxComponentForCollision(MWConComp->BoxComponentLeftRear, MWConComp->WheelLeftRear, CollisionMaterial);
		AdjustBoxComponentForCollision(MWConComp->BoxComponentRightRear, MWConComp->WheelRightRear, CollisionMaterial);

		return true;
	}
	return false;
}

// Setup the BoxComponents.
bool MWControllerBoxComponentHandler::AdjustBoxComponentForCollision(UBoxComponent* BoxComp, UStaticMeshComponent* MeshComp, UPhysicalMaterial* ColMat)
{
	if (BoxComp && MeshComp && ColMat)
	{
		// The BoxComponents are attached to the base or to the BaseBoxBomponent so that they behave correctly.
		if (BoxComp == MWConComp->BoxComponentBase)
		{
			BoxComp->AttachToComponent(MWConComp->Base, FAttachmentTransformRules(EAttachmentRule::KeepRelative, false));
		}
		else
		{
			BoxComp->AttachToComponent(MWConComp->BoxComponentBase, FAttachmentTransformRules(EAttachmentRule::KeepRelative, false));
		}

		// Reg and Init the components so the editor can find them and that they work correctly. 
		BoxComp->RegisterComponent();
		BoxComp->InitializeComponent();

		BoxComp->SetPhysMaterialOverride(ColMat);

		// We do not want to see them on playtime. 
		BoxComp->bHiddenInGame = true;

		BoxComp->ComponentTags.Add("MWBoxCollisionTag");

		// Uses the values from the static mesh (rotation) so that the BoundingBox is not calculated incorrectly (size/extent).
		// Calculates the position based on the components, eliminating rotation errors.
		BoxComp->SetBoxExtent(MeshComp->GetStaticMesh()->GetBoundingBox().GetExtent()* SCALE_FACTOR_BOXCOMP);
		BoxComp->SetWorldLocationAndRotation(MeshComp->Bounds.Origin, MeshComp->GetComponentRotation());

		// Sets the collision profile. 
		BoxComp->SetCollisionObjectType(ECC_MW);
		BoxComp->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
		BoxComp->SetCollisionResponseToChannel(ECC_MW, ECollisionResponse::ECR_Ignore);
		BoxComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		BoxComp->SetSimulatePhysics(false);

		// Gravity is off, otherwise the velocity functions will cause problems.
		BoxComp->SetEnableGravity(false);

		// So that the BoxComponents are at least partially editable.
		BoxComp->bEditableWhenInherited = true;

		// Saves the references if they are lost in the editor. They can still be deleted (this is not about the editor because they are inherited).
		MWConComp->CreatedBoxComponentsList.Add(BoxComp->GetName(), BoxComp);

		return true;
	}
	return false;
}

// Destroys the BoxComponents and old references.
void MWControllerBoxComponentHandler::DestroyBoxComponents()
{
	if (CheckBase() && CheckAllWheels())
	{
		if (MWConComp->BoxComponentLeftFront && MWConComp->BoxComponentLeftFront->IsValidLowLevel())
		{
			MWConComp->BoxComponentLeftFront->DestroyComponent();
			MWConComp->BoxComponentLeftFront = nullptr;
			UE_LOG(LogTemp, Log, TEXT("[%s][%d]. %s was deleted."), TEXT(__FUNCTION__), __LINE__, *MWConComp->WheelNameLeftFront);
		}
		if (MWConComp->BoxComponentRightFront && MWConComp->BoxComponentRightFront->IsValidLowLevel())
		{
			UE_LOG(LogTemp, Log, TEXT("[%s][%d]. %s was deleted."), TEXT(__FUNCTION__), __LINE__, *MWConComp->WheelNameRightFront);
			MWConComp->BoxComponentRightFront->DestroyComponent();
			MWConComp->BoxComponentRightFront = nullptr;
		}
		if (MWConComp->BoxComponentLeftRear && MWConComp->BoxComponentLeftRear->IsValidLowLevel())
		{
			MWConComp->BoxComponentLeftRear->DestroyComponent();
			MWConComp->BoxComponentLeftRear = nullptr;
			UE_LOG(LogTemp, Log, TEXT("[%s][%d]. %s was deleted."), TEXT(__FUNCTION__), __LINE__, *MWConComp->WheelNameLeftRear);
		}
		if (MWConComp->BoxComponentRightRear && MWConComp->BoxComponentRightRear->IsValidLowLevel())
		{
			MWConComp->BoxComponentRightRear->DestroyComponent();
			MWConComp->BoxComponentRightRear = nullptr;
			UE_LOG(LogTemp, Log, TEXT("[%s][%d]. %s was deleted."), TEXT(__FUNCTION__), __LINE__, *MWConComp->WheelNameRightRear);
		}
		if (MWConComp->BoxComponentBase && MWConComp->BoxComponentBase->IsValidLowLevel())
		{
			MWConComp->BoxComponentBase->DestroyComponent();
			MWConComp->BoxComponentBase = nullptr;
			UE_LOG(LogTemp, Log, TEXT("[%s][%d]. %s was deleted."), TEXT(__FUNCTION__), __LINE__, *MWConComp->BaseName);
		}
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("[%s][%d] this was nullptr. Can not destroy BoxComponents "), TEXT(__FUNCTION__), __LINE__);
	}

	// Destroys BoxComponents that are no longer accessible via the editor.
	if (MWConComp) {
		for (auto Box : MWConComp->CreatedBoxComponentsList)
		{
			Box.Value->DestroyComponent();
		}
	}
}
