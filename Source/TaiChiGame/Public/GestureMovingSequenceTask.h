// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "CustomHandComponent.h"
#include "GestureMovingSequenceTask.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGestureMovingSequenceOutput, int, CurrentIndex);

/**
 *
 */
UCLASS()
class TAICHIGAME_API UGestureMovingSequenceTask : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	virtual void Activate() override;

	UPROPERTY(BlueprintAssignable)
		FGestureMovingSequenceOutput OnNextGestureDetected;

	UPROPERTY(BlueprintAssignable)
		FGestureMovingSequenceOutput OnGestureSequenceCancelled;

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
		static UGestureMovingSequenceTask* TrackOneHandedMovingGestureSequence(UCustomHandComponent* HandComp, TArray<FName> GestureSequence, float ResetTime = 3.f);

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
		static UGestureMovingSequenceTask* TrackTwoHandedMovingGestureSequence(UCustomHandComponent* LeftHandComp, UCustomHandComponent* RightHandComp, TArray<FName> GestureSequence, float ResetTime = 3.f);

	UFUNCTION()
		void OnGestureDetected(TArray<FName> GestureNames);

	UFUNCTION()
		void OnLeftGestureDetected(TArray<FName> GestureNames);

	UFUNCTION()
		void OnRightGestureDetected(TArray<FName> GestureNames);

	UFUNCTION()
		void GestureSequenceCancelled();

	TArray<FName> GestureNameSequenceArray;

	FTimerHandle CancelGestureSequenceTimer;

	float TimerInterval = 3.f;

	UCustomHandComponent* HandCompObject = nullptr;
	UCustomHandComponent* LeftHandCompObject = nullptr;
	UCustomHandComponent* RightHandCompObject = nullptr;

private:
	int CurrentGestureIndex = 0;

	float MaxHandDistanceToNextGesture = 0.f;
	FVector NextTargetLocation = FVector();

	bool LeftGestureActivated = false;
	bool RightGestureActivated = false;

};
