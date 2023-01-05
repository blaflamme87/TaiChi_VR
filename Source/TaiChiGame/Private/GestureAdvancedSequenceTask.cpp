// Fill out your copyright notice in the Description page of Project Settings.

#include "GestureAdvancedSequenceTask.h"
#include "Kismet/GameplayStatics.h"


void UGestureAdvancedSequenceTask::Activate()
{
	HandCompObject->OnGesturesDetected.AddDynamic(this, &UGestureAdvancedSequenceTask::OnGestureDetected);
}

UGestureAdvancedSequenceTask* UGestureAdvancedSequenceTask::TrackAdvancedGestureSequence(UCustomHandComponent* HandComp, TArray<FName> GestureSequence, float ResetTime)
{
	if (!HandComp) { return nullptr; }

	UGestureAdvancedSequenceTask* GestureSequenceTask = NewObject<UGestureAdvancedSequenceTask>();
	GestureSequenceTask->HandCompObject = HandComp;
	GestureSequenceTask->GestureNameSequenceArray = GestureSequence;
	GestureSequenceTask->TimerInterval = ResetTime;

	return GestureSequenceTask;
}

void UGestureAdvancedSequenceTask::OnGestureDetected(TArray<FName> GestureNames)
{
	if (!HandCompObject || !GestureNameSequenceArray[CurrentGestureIndex].IsValid()) { return; }

	for (FName GestureName : GestureNames)
	{
		if (GestureName == GestureNameSequenceArray[CurrentGestureIndex])
		{
			OnNextGestureDetected.Broadcast(CurrentGestureIndex);
			CurrentGestureIndex++;



			if (GestureNameSequenceArray.IsValidIndex(CurrentGestureIndex))
			{
				UE_LOG(LogTemp, Warning, TEXT("Gesture index is %i and its valid"), CurrentGestureIndex);

				HandCompObject->GetWorld()->GetTimerManager().ClearTimer(CancelGestureSequenceTimer);
				HandCompObject->GetWorld()->GetTimerManager().SetTimer(CancelGestureSequenceTimer, this, &UGestureAdvancedSequenceTask::GestureSequenceCancelled, TimerInterval, false, TimerInterval);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Gesture index is %i and its NOT valid   "), CurrentGestureIndex);

				HandCompObject->GetWorld()->GetTimerManager().ClearTimer(CancelGestureSequenceTimer);
				CurrentGestureIndex = 0;
			}
		}
	}


}

void UGestureAdvancedSequenceTask::GestureSequenceCancelled()
{
	CurrentGestureIndex = 0;
	OnGestureSequenceCancelled.Broadcast(-1);
	HandCompObject->GetWorld()->GetTimerManager().ClearTimer(CancelGestureSequenceTimer);
}


