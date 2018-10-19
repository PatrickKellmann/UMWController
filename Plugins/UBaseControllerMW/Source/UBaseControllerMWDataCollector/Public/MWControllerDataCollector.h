// Copyright 2018, Institute for Artificial Intelligence - University of Bremen
// Author: Patrick Kellmann

#pragma once

#include "Engine.h"
#include "Components/ActorComponent.h"
#include "MWControllerComponent.h"
#include "MWControllerDataCollector.generated.h"

/*
* This class stores data from a MWController which is searched in the owner.
* The stored data are obtained via getter. The data itself is recorded in a csv file, which stores either in each tick (high-performance) or every second.
*/
UCLASS(EditInlineNew, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UBASECONTROLLERMWDATACOLLECTOR_API UMWControllerDataCollector : public UActorComponent
{
	GENERATED_BODY()

public:
	/*
	*Sets default values for this component's properties
	*/
	UMWControllerDataCollector();

	/*
	* Checks every tick if the specified time has come to start a data collection and initiates the writing. 
	* It is checked whether it should continue to be written. In case of problems, the tick is suspended.
	*/
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


	/*
	* DestroyComponent destroyed MWControllerDataCollector and set all elements to null.
	*
	* @param bPromoteChildren Indicates if child should be promoted.
	*/
	virtual void DestroyComponent(bool bPromoteChildren) override;

protected:
	/*
	* Called when the game starts, search for the MWController in the owner.
	* Creates folder structure and file.
	*/
	virtual void BeginPlay() override;

	/*
	* Called when the game starts or at BeginPlay and sets the owner. 
	*/
	virtual void PostInitProperties() override;

private:

	/*
	* Creates the folder structure for the csv file.
	*
	* @return return true if folder could be created. Otherwise false.
	*/
	bool CreateDataDir();

	/*
	* Creates a csv file in which data is stored per tick/every second. 
	* It is specified what is in the first line (data type). The file is named after the time.
	*
	* @return returns true if the file could be created. Otherwise false.
	*/
	bool CreateFile();

	/*
	* Writes the corresponding data into the created csv file. Here the getters are called.
	*
	* @return returns true if it could be written to the file. Otherwise false.
	*/
	bool WriteInLine();


	/*
	* Called after the end of the game and releases the file.
	*/
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override; 

	//Owner of the MWController and the DataCollector. 
	AActor* MWRobotBaseActor = nullptr;

	// MWController 
	UMWControllerComponent* MWConComp = nullptr;

	// For searching and creation of the folder. 
	IPlatformFile* PlatformFile = nullptr;

	// Saves the generated path for the folder. Every day a new folder is created.
	FString DataDir;

	// Saves the path of the csv file.
	FString FilePath;

	// File handle to process the raw data
	IFileHandle* FileHandle;
};
