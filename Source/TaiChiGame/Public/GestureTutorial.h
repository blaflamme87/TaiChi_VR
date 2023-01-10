// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CustomHandComponent.h"
#include "GestureTutorial.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FGestureSequenceDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGestureIndexDelegate, int, GestureIndex);

USTRUCT(BlueprintType)
struct FGestureTutorialData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCustomHandComponent* LeftHand;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCustomHandComponent* RightHand;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FName> SequenceGestureNames;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AGestureTutorial* GestureTutorial;

};

UCLASS(BlueprintType)
class TAICHIGAME_API AGestureTutorial : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGestureTutorial();

	void SetupTutorial(FGestureTutorialData TutorialData);

	UPROPERTY(BlueprintAssignable)
	FGestureIndexDelegate OnNextGestureDetected;

	UPROPERTY(BlueprintAssignable)
	FGestureSequenceDelegate OnGestureSequenceCompleted;

	UPROPERTY(BlueprintAssignable)
	FGestureSequenceDelegate OnGestureSequenceCancelled;

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USceneComponent> Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UPoseableMeshComponent> TutorialHandLeft;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UPoseableMeshComponent> TutorialHandRight;
 
	UPROPERTY(EditAnywhere)
	float BoneLocationThreshold = 10.f;

	UPROPERTY(EditAnywhere)
	float DetectHandPoseUpdateInterval = .1f;

	UPROPERTY(EditAnywhere)
	float HandDetectionExpiredTime = 10.f;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void StartGestureSequenceTutorial(UCustomHandComponent* CustomHandLeft, UCustomHandComponent* CustomHandRight, TArray<FName> SequenceGestureNames);


private:
	
	FTimerHandle DetectHandPoseUpdateTimer;
	FTimerHandle HandResetTimer;

	UCustomHandComponent* CurrentHandLeft = nullptr;
	UCustomHandComponent* CurrentHandRight = nullptr;

	TArray<FName> GestureNames;

	FGesture GestureLeft;
	FGesture GestureRight;

	int CurrentGestureIndex = 0;

	FTransform OriginalCamTransform;

	UFUNCTION()
	void TrackGesture(FName GestureName);

	UFUNCTION()
	void PoseHand(UPoseableMeshComponent* HandToPose, FGesture GestureData);

	UFUNCTION()
	void CheckHandsUpdate();

	UFUNCTION()
	bool CheckHand(UPoseableMeshComponent* HandToMatch, UCustomHandComponent* HandToCheck);

	UFUNCTION()
	void ResetTimerExpired();

	UFUNCTION()
	void SequenceCompleted();

};
