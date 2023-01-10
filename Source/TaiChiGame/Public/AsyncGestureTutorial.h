// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "CustomHandComponent.h"
#include "GestureTutorial.h"
#include "AsyncGestureTutorial.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FGestureSequenceDelegatez);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGestureIndexDelegatez, int, GestureIndex);


/**
 * 
 */
UCLASS()
class TAICHIGAME_API UAsyncGestureTutorial : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	virtual void Activate() override;

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	static UAsyncGestureTutorial* RunGestureTutorial(FGestureTutorialData TutorialData);

	UPROPERTY(BlueprintAssignable)
	FGestureIndexDelegatez OnNextGestureDetected;

	UPROPERTY(BlueprintAssignable)
	FGestureSequenceDelegatez OnGestureSequenceCompleted;

	UPROPERTY(BlueprintAssignable)
	FGestureSequenceDelegatez OnGestureSequenceCancelled;

	AGestureTutorial* GestureTutorial;

	UCustomHandComponent* PlayerLeftHand;
	UCustomHandComponent* PlayerRightHand;
	TArray<FName> GestureNames;

	UFUNCTION()
	void NextGestureDetected(int Index);

	UFUNCTION()
	void GestureSequenceCompleted();

	UFUNCTION()
	void GestureSequenceCancelled();
	
};
