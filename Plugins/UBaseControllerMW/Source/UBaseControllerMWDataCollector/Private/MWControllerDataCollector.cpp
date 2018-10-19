// Copyright 2018, Institute for Artificial Intelligence - University of Bremen
// Author: Patrick Kellmann

#include "MWControllerDataCollector.h"
#include "Misc/DateTime.h"

// Sets default values for this component's properties
UMWControllerDataCollector::UMWControllerDataCollector()
{
	// Sets this component to be initialized when the game starts, and to be ticked every frame.  
	//You can turn these features off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}

// Prepares the DataCollector and sets the owner. 
void UMWControllerDataCollector::PostInitProperties()
{
	Super::PostInitProperties();
}

// Destroy the object with all components.
void UMWControllerDataCollector::DestroyComponent(bool bPromoteChildren)
{
	Super::DestroyComponent(bPromoteChildren);

	MWRobotBaseActor = nullptr;
	MWConComp = nullptr;
	PlatformFile = nullptr;

	if (FileHandle)
	{
		delete FileHandle;
	}
}

// Called when the game starts
void UMWControllerDataCollector::BeginPlay()
{
	Super::BeginPlay();

	if (this && !this->IsDefaultSubobject())
	{
		MWRobotBaseActor = this->GetOwner();
	}

	//Produces and verifies each Begin Play the folder structure.
	if (MWRobotBaseActor && MWRobotBaseActor->IsValidLowLevel())
	{
		TArray<UMWControllerComponent*> ActorComponentList;
		MWRobotBaseActor->GetComponents<UMWControllerComponent>(ActorComponentList);

		for (auto ActComp : ActorComponentList)
		{
			//Searches for the controller per tag. 
			if (ActComp->ComponentHasTag("MWControllerComponentTag"))
			{
				MWConComp = ActComp;
				if (MWConComp && MWConComp->IsValidLowLevel())
				{
					PlatformFile = &FPlatformFileManager::Get().GetPlatformFile();

					if (!CreateDataDir())
					{
						UE_LOG(LogTemp, Error,
							TEXT("[%s][%d] Could not create director DataCollection. Tick of UMWControllerDataCollector will be turn off."),
							TEXT(__FUNCTION__), __LINE__);

						this->SetComponentTickEnabled(false);
						break;
					}

					if (!CreateFile())
					{
						UE_LOG(LogTemp, Error,
							TEXT("[%s][%d] Could not create .csv file. Tick of UMWControllerDataCollector will be turn off."),
							TEXT(__FUNCTION__), __LINE__);

						this->SetComponentTickEnabled(false);
						break;
					}
				}
				break;
			}
		}
	}
}

// Called every frame
void UMWControllerDataCollector::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// If MWController no longer ticks or has been destroyed in any way, data should no longer be written to the file.
	if (!MWConComp || !MWConComp->IsValidLowLevel() || !MWConComp->IsComponentTickEnabled() || MWConComp->IsBeingDestroyed())
	{
		this->SetComponentTickEnabled(false);
		UE_LOG(LogTemp, Error,
			TEXT("[%s][%d] No MWController was found or it is turn off. Tick of UMWControllerDataCollector will be turn off."),
			TEXT(__FUNCTION__), __LINE__);

	}
	else
	{
		if (!WriteInLine())
		{
			UE_LOG(LogTemp, Error,
				TEXT("[%s][%d] File %s do not exists. Tick of UMWControllerDataCollector will be turn off."),
				TEXT(__FUNCTION__), __LINE__, *FilePath);

			this->SetComponentTickEnabled(false);
		}

	}
}

//Creates a folder structure for files /Project/DataCollection/DAY
bool UMWControllerDataCollector::CreateDataDir()
{
	const FDateTime Date = FDateTime::Now();

	DataDir = FPaths::ProjectDir()
		+ "DataCollection/"
		+ FString::FromInt(Date.GetYear())
		+ "." + FString::FromInt(Date.GetMonth())
		+ "." + FString::FromInt(Date.GetDay());

	//Creates DataCollection folder if missing.
	if (!PlatformFile->DirectoryExists(*FString(FPaths::ProjectDir() + "DataCollection/")))
	{
		PlatformFile->CreateDirectory(*FString(FPaths::ProjectDir() + "DataCollection/"));
	}

	//Creates folder for .csv files with date reference.
	if (!PlatformFile->DirectoryExists(*DataDir))
	{
		PlatformFile->CreateDirectory(*DataDir);
	}

	return PlatformFile->DirectoryExists(*DataDir);
}

//Creates .csv file and defines the data fields. 
bool UMWControllerDataCollector::CreateFile()
{
	const FDateTime Time = FDateTime::Now();
	FilePath = DataDir + "/"
		+ "MW_Data_" + FString::FromInt(Time.GetHour())
		+ "_" + FString::FromInt(Time.GetMinute())
		+ TEXT(".csv");

		FString FirstLine = (FString("Time(BeginPlay); MW_Longitudinal_Velocity; MW_Transversal_Velocity; MW_Angular_Velocity; MW_Wheel_Left_Front(rad/s); MW_Wheel_Right_Front(rad/s); MW_Wheel_Left_Rear(rad/s); MW_Wheel_RightRear(rad/s); \n"));
		FileHandle = FPlatformFileManager::Get().GetPlatformFile().OpenWrite(*FilePath, true);

		return FileHandle->Write((const uint8*)TCHAR_TO_ANSI(*FirstLine), FirstLine.Len());
}

// Writes a new line.
bool UMWControllerDataCollector::WriteInLine()
{
	if (PlatformFile->FileExists(*FilePath))
	{
		//FJsonSerializableArray NextLine;
		FString NextLine;

		//Uses time format to synchronize with data.
		float BeginTime = this->GetWorld()->GetTimeSeconds();
		FString TimeStamp = FString::SanitizeFloat(BeginTime);

		const FString Wheel1 = FString::SanitizeFloat(MWConComp->GetWheelLeftFrontAngularVelocity());
		const FString Wheel2 = FString::SanitizeFloat(MWConComp->GetWheelRightFrontAngularVelocity());
		const FString Wheel3 = FString::SanitizeFloat(MWConComp->GetWheelLeftRearAngularVelocity());
		const FString Wheel4 = FString::SanitizeFloat(MWConComp->GetWheelRightRearAngularVelocity());

		// Conversion from cm to meter
		const FString MWLongVel = FString::SanitizeFloat(MWConComp->GetBaseLongitudinalVelocity() / SCALE_FACTOR_CM_TO_M);
		const FString MWTransVel = FString::SanitizeFloat(MWConComp->GetBaseTransversalVelocity() / SCALE_FACTOR_CM_TO_M);

		// rad/s
		const FString MWAngVel = FString::SanitizeFloat(MWConComp->GetBaseAngularVelocity());

		NextLine = (FString(TimeStamp + ";" + MWLongVel + ";" + MWTransVel + ";" + MWAngVel + ";" + Wheel1 + ";"
			+ Wheel2 + ";" + Wheel3 + ";" + Wheel4 + "\n").Replace(TEXT("."), TEXT(","), ESearchCase::IgnoreCase));

		// Writes in file.
		return FileHandle->Write((const uint8*)TCHAR_TO_ANSI(*NextLine), NextLine.Len());
	}
	return false;
}

// Deletes FileHandle so that the file is released.
void UMWControllerDataCollector::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (FileHandle)
	{
		delete FileHandle;
	}
}
