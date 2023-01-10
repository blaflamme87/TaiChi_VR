// Fill out your copyright notice in the Description page of Project Settings.


#include "AsyncGestureTutorial.h"
#include "Kismet/GameplayStatics.h"


void UAsyncGestureTutorial::Activate()
{
	if (PlayerLeftHand && PlayerRightHand && GestureTutorial)
	{
		GestureTutorial->StartGestureSequenceTutorial(PlayerLeftHand, PlayerRightHand, GestureNames);
		GestureTutorial->OnNextGestureDetected.AddDynamic(this, &UAsyncGestureTutorial::NextGestureDetected);
		GestureTutorial->OnGestureSequenceCompleted.AddDynamic(this, &UAsyncGestureTutorial::GestureSequenceCompleted);
		GestureTutorial->OnGestureSequenceCancelled.AddDynamic(this, &UAsyncGestureTutorial::GestureSequenceCancelled);

		//GestureTutorial->StartGestureSequenceTutorial(PlayerLeftHand, PlayerRightHand, GestureNames);

		NextGestureDetected(17);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("tezt Somethin was missing on node::Activate()"));
		SetReadyToDestroy();
	}
}

UAsyncGestureTutorial* UAsyncGestureTutorial::RunGestureTutorial(FGestureTutorialData TutorialData)
{
	if (!(TutorialData.LeftHand && TutorialData.RightHand)) { return nullptr; }

	UAsyncGestureTutorial* GestureTutorialTask = NewObject<UAsyncGestureTutorial>();
	GestureTutorialTask->PlayerLeftHand = TutorialData.LeftHand;
	GestureTutorialTask->PlayerRightHand = TutorialData.RightHand;
	GestureTutorialTask->GestureNames = TutorialData.SequenceGestureNames;
	//GestureTutorialTask->GestureTutorial = Cast<AGestureTutorial>(UGameplayStatics::GetActorOfClass(GestureTutorialTask->PlayerLeftHand->GetWorld(), AGestureTutorial::StaticClass()));
	GestureTutorialTask->GestureTutorial = TutorialData.GestureTutorial;

	return GestureTutorialTask;
}

void UAsyncGestureTutorial::NextGestureDetected(int Index)
{
	UE_LOG(LogTemp, Warning, TEXT("tezt we detected the next gesjya"));
	OnNextGestureDetected.Broadcast(Index);
}

void UAsyncGestureTutorial::GestureSequenceCompleted()
{
	OnGestureSequenceCompleted.Broadcast();
	SetReadyToDestroy();
}

void UAsyncGestureTutorial::GestureSequenceCancelled()
{
	OnGestureSequenceCancelled.Broadcast();
	SetReadyToDestroy();
}
