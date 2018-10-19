// Copyright 2018, Institute for Artificial Intelligence - University of Bremen
// Author: Patrick Kellmann

#include "MWControllerDemoPawn.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AMWControllerDemoPawn::AMWControllerDemoPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	//Erzeugt Struktur, SpringArm und Camera
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	MWDemoCameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("MWDemoCameraBoom"));
	MWDemoFollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("MWDemoFollowCamera"));

	MWDemoCameraBoom->SetupAttachment(RootComponent);

	// Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	MWDemoFollowCamera->SetupAttachment(MWDemoCameraBoom, USpringArmComponent::SocketName); 

	// Camera does not rotate relative to arm
	MWDemoFollowCamera->bUsePawnControlRotation = false; 

	AutoPossessPlayer = EAutoReceiveInput::Player0;

	// Sets the distance of the camera. 
	// Can be adjusted if necessary.
	MWDemoFollowCamera->SetRelativeLocation(FVector(-250.f, 0.f, 20.f));
}


// Called when the game starts or when spawned
void AMWControllerDemoPawn::BeginPlay()
{
	Super::BeginPlay();

	if (this && this->IsValidLowLevel())
	{
		// Get the actor who holds the robot.
		TArray<AActor*> HelperList;
		UGameplayStatics::GetAllActorsOfClass(this->GetWorld(), AActor::StaticClass(), HelperList);
		for (auto RobAct : HelperList)
		{
			// Looks for the default names. "Contains" is used because Unreal changes the name in some cases.
			if (RobAct->GetName().Contains(MWRobotBaseActorName))
			{
				if (RobAct && RobAct->IsValidLowLevel()) 
				{
					MWRobotBaseActor = RobAct;
					this->SetActorLocation(MWRobotBaseActor->GetActorLocation());
					this->SetActorRotation(MWRobotBaseActor->GetActorRotation());

					// Attach to MWRobotBaseActor holding the robot. 
					this->AttachToActor(MWRobotBaseActor, FAttachmentTransformRules::KeepWorldTransform);
					break;
				}
			}
		}

		if (!MWRobotBaseActor ||!MWRobotBaseActor->IsValidLowLevel()) 
		{
			UE_LOG(LogTemp, Error,
				TEXT("[%s][%d].MWRobotBaseActor was nullptr. Colud not attach pawn to %s."),
				TEXT(__FUNCTION__), __LINE__, *MWRobotBaseActorName);
		}
	}
}

// Called every frame
void AMWControllerDemoPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AMWControllerDemoPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

