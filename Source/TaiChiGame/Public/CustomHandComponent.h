// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OculusHandComponent.h"
#include "GestureStruct.h"
#include "GestureData.h"
#include "GestureSaveGame.h"
#include "CustomHandComponent.generated.h"


UENUM(BlueprintType)
enum E_GrabState
{
	Grabbed,
	Dropped
};

UENUM(BlueprintType)
enum E_TriggerState
{
	Pulled,
	Released
};

UENUM(BlueprintType)
enum E_GestureLoadSource
{
	FromDataTable,
	FromSaveGameObject
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGestureDelegate, TArray<FName>, GestureNames);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGrabDelegate, E_GrabState, Grab_State);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTriggerDelegate, E_TriggerState, Trigger_State);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAxisDelegate, float, Axis);

/**
 *
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class TAICHIGAME_API UCustomHandComponent : public UOculusHandComponent
{
	GENERATED_BODY()

public:
	UCustomHandComponent();

protected:

	virtual void BeginPlay() override;

public:

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/** Where we load the gestures from */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gestures")
	TEnumAsByte<E_GestureLoadSource> GestureLoadSource = E_GestureLoadSource::FromDataTable;

	UPROPERTY(BlueprintAssignable, Category = "Gestures")
	FGestureDelegate ActiveGesturesUsed;

	UPROPERTY(BlueprintAssignable, Category = "Gestures")
	FGestureDelegate OnGesturesDetected;

	UPROPERTY(BlueprintAssignable, Category = "Grab and Trigger")
	FGrabDelegate OnGrab;

	UPROPERTY(BlueprintAssignable, Category = "Grab and Trigger")
	FTriggerDelegate OnPullTrigger;

	UPROPERTY(BlueprintAssignable, Category = "Grab and Trigger")
	FAxisDelegate OnGrabAxis;

	UPROPERTY(BlueprintAssignable, Category = "Grab and Trigger")
	FAxisDelegate OnPullTriggerAxis;

	/** Data table with the Left Hand gestures */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gestures")
	UDataTable* GesturesLeftDataTable;

	/** Data table with the Right Hand gestures */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gestures")
	UDataTable* GesturesRightDataTable;

	/** The name of the save slot used for the gestures */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gestures")
	FString GestureGameSlotName;

	UPROPERTY(BlueprintReadWrite)
	UGestureSaveGame* GestureSaveGameObject = nullptr;

	UPROPERTY(BlueprintReadWrite)
	TArray<FGesture> Gestures;

	/** The Timer interval for detecting gestures. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gestures")
	float GestureDetectionRefreshRate = 0.1f;

	/** the Timer interval for detecting grabbing and pulling trigger */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grab and Trigger")
	float GrabAndPointerRefreshRate = 0.033f;

	/** The distance tolerance for bone locations in the hand compared to stored gesture locations */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gestures")
	float GestureThreshold = 3.0f;

	/** the distance tolerance for the location of the root bone for gestures that use RootTransformMatters */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gestures")
	float GestureRootLocationThreshold = 10.0f;

	/** the Dot Product threshold for the rotation of the root bone for gestures that use RootTransformMatters */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gestures", meta = (ClampMin = -1, ClampMax = 1))
	float GestureRootRotationDotThreshold = .7f;

	UPROPERTY(BlueprintReadOnly)
	TArray<FGesture> CurrentGestures;

	UPROPERTY(BlueprintReadOnly)
	TArray<FGesture> CurrentActiveGestures;

	/** the threshold for GrabAxis to do a grab */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grab and Trigger", meta = (ClampMin = 0, ClampMax = 1))
	float GrabThreshold = .65f;

	UPROPERTY(BlueprintReadOnly)
	bool bIsGrabbing = false;

	UPROPERTY(BlueprintReadOnly)
	float GrabAxis = 0.f;

	/** the threshold for PullTriggerAxis to pull the trigger */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grab and Trigger", meta = (ClampMin = 0, ClampMax = 1))
	float PullTriggerThreshold = .65f;

	UPROPERTY(BlueprintReadOnly)
	bool bIsPullingTrigger = false;

	UPROPERTY(BlueprintReadOnly)
	float PullTriggerAxis = 0.f;

	/** the "contains string" we need for finding all the fingertips */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grab and Trigger")
	FString FingertipString = "Tip";

	/** the "contains string" we need to find the index finger */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grab and Trigger")
	FString IndexFingerString = "index";

	/** the range of a finger's distance from the root bone that will map to a 0-1 for GrabAxis and PullTriggerAxis */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grab and Trigger")
	FFloatRange FingerDistRange = FFloatRange(16.f, 8.f);

	/** the amount of time needed to go by to cancel a gesture, this prevents flickering of gestures */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gestures")
	float GestureLeewayThreshold = .5f;

private:
	FTimerHandle DetectGesturesTimerHandle;
	FTimerHandle UpdateGrabAndPointerTimerHandle;

	float CurrentLeewayTimeElapsed = 0.f;


public:
	UFUNCTION()
	UGestureSaveGame* LoadGesturesSaveObject();

	UFUNCTION()
	TArray<FGesture> LoadGesturesFromDataTable();

	UFUNCTION()
	bool DoesGestureExist(UGestureSaveGame* SaveObject, FName GestureName);

	UFUNCTION(BlueprintCallable)
	void RecordGesture(FName GestureName, bool RootTransformMatters = false);

	UFUNCTION(BlueprintCallable)
	FGesture RecordGestureToDisk(FName GestureName);

	UFUNCTION(BlueprintCallable)
	void DeleteGesture(FName GestureName);

	UFUNCTION(BlueprintCallable)
	TArray<FGesture> DetectCurrentGestures();

	UFUNCTION(BlueprintCallable)
	FGesture GetGestureFromName(FName GestureName) const;

	UFUNCTION()
	void DetectGesturesOnTimer();

	UFUNCTION()
	void UpdateGrabAndPointerOnTimer();

protected:

	UFUNCTION()
	bool SetGrabAndPointerAxes();

};
