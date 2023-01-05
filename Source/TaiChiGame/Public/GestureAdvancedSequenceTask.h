// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "CustomHandComponent.h"
#include "GestureAdvancedSequenceTask.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGestureAdvancedSequenceOutput, int, CurrentIndex);

/**
 *
 */
UCLASS()
class TAICHIGAME_API UGestureAdvancedSequenceTask : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	virtual void Activate() override;

	UPROPERTY(BlueprintAssignable)
		FGestureAdvancedSequenceOutput OnNextGestureDetected;

	UPROPERTY(BlueprintAssignable)
		FGestureAdvancedSequenceOutput OnGestureSequenceCancelled;

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
		static UGestureAdvancedSequenceTask* TrackAdvancedGestureSequence(UCustomHandComponent* HandComp, TArray<FName> GestureSequence, float ResetTime = 3.f);

	UFUNCTION()
		void OnGestureDetected(TArray<FName> GestureNames);

	UFUNCTION()
		void GestureSequenceCancelled();

	TArray<FName> GestureNameSequenceArray;

	FTimerHandle CancelGestureSequenceTimer;

	float TimerInterval = 3.f;

	UCustomHandComponent* HandCompObject = nullptr;

private:
	int CurrentGestureIndex = 0;

	float MaxHandDistanceToNextGesture = 0.f;
	FVector NextTargetLocation = FVector();

};
