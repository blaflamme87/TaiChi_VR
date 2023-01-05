// Fill out your copyright notice in the Description page of Project Settings.

#include "GestureMovingSequenceTask.h"
#include "Kismet/GameplayStatics.h"


void UGestureMovingSequenceTask::Activate()
{
	if (LeftHandCompObject && RightHandCompObject)
	{
		LeftHandCompObject->OnGesturesDetected.AddDynamic(this, &UGestureMovingSequenceTask::OnLeftGestureDetected);
		RightHandCompObject->OnGesturesDetected.AddDynamic(this, &UGestureMovingSequenceTask::OnRightGestureDetected);
	}
	else if (HandCompObject)
	{
		HandCompObject->OnGesturesDetected.AddDynamic(this, &UGestureMovingSequenceTask::OnGestureDetected);
	}


}

UGestureMovingSequenceTask* UGestureMovingSequenceTask::TrackOneHandedMovingGestureSequence(UCustomHandComponent* HandComp, TArray<FName> GestureSequence, float ResetTime)
{
	if (!HandComp) { return nullptr; }

	UGestureMovingSequenceTask* GestureSequenceTask = NewObject<UGestureMovingSequenceTask>();
	GestureSequenceTask->HandCompObject = HandComp;
	GestureSequenceTask->GestureNameSequenceArray = GestureSequence;
	GestureSequenceTask->TimerInterval = ResetTime;

	return GestureSequenceTask;
}

UGestureMovingSequenceTask* UGestureMovingSequenceTask::TrackTwoHandedMovingGestureSequence(UCustomHandComponent* LeftHandComp, UCustomHandComponent* RightHandComp, TArray<FName> GestureSequence, float ResetTime)
{
	if (!LeftHandComp || !RightHandComp) { return nullptr; }

	UGestureMovingSequenceTask* GestureSequenceTask = NewObject<UGestureMovingSequenceTask>();
	GestureSequenceTask->LeftHandCompObject = LeftHandComp;
	GestureSequenceTask->RightHandCompObject = RightHandComp;
	GestureSequenceTask->GestureNameSequenceArray = GestureSequence;
	GestureSequenceTask->TimerInterval = ResetTime;

	return GestureSequenceTask;
}

void UGestureMovingSequenceTask::OnGestureDetected(TArray<FName> GestureNames)
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
				HandCompObject->GetWorld()->GetTimerManager().ClearTimer(CancelGestureSequenceTimer);
				HandCompObject->GetWorld()->GetTimerManager().SetTimer(CancelGestureSequenceTimer, this, &UGestureMovingSequenceTask::GestureSequenceCancelled, TimerInterval, false, TimerInterval);
			}
			else
			{
				HandCompObject->GetWorld()->GetTimerManager().ClearTimer(CancelGestureSequenceTimer);
				CurrentGestureIndex = 0;
			}
		}
	}


}

void UGestureMovingSequenceTask::OnLeftGestureDetected(TArray<FName> GestureNames)
{
	if (!LeftHandCompObject || !GestureNameSequenceArray[CurrentGestureIndex].IsValid()) { return; }

	for (FName GestureName : GestureNames)
	{
		if (GestureName == GestureNameSequenceArray[CurrentGestureIndex])
		{
			LeftGestureActivated = true;

			if (RightGestureActivated && LeftGestureActivated)
			{
				LeftGestureActivated = false;
				RightGestureActivated = false;
				OnNextGestureDetected.Broadcast(CurrentGestureIndex);
				CurrentGestureIndex++;

				if (GestureNameSequenceArray.IsValidIndex(CurrentGestureIndex))
				{
					LeftHandCompObject->GetWorld()->GetTimerManager().ClearTimer(CancelGestureSequenceTimer);
					LeftHandCompObject->GetWorld()->GetTimerManager().SetTimer(CancelGestureSequenceTimer, this, &UGestureMovingSequenceTask::GestureSequenceCancelled, TimerInterval, false, TimerInterval);
				}
				else
				{
					LeftHandCompObject->GetWorld()->GetTimerManager().ClearTimer(CancelGestureSequenceTimer);
					CurrentGestureIndex = 0;
				}
			}
		}
	}


}

void UGestureMovingSequenceTask::OnRightGestureDetected(TArray<FName> GestureNames)
{
	if (!RightHandCompObject || !GestureNameSequenceArray[CurrentGestureIndex].IsValid()) { return; }

	for (FName GestureName : GestureNames)
	{
		if (GestureName == GestureNameSequenceArray[CurrentGestureIndex])
		{
			RightGestureActivated = true;

			if (LeftGestureActivated && RightGestureActivated)
			{
				LeftGestureActivated = false;
				RightGestureActivated = false;
				OnNextGestureDetected.Broadcast(CurrentGestureIndex);
				CurrentGestureIndex++;

				if (GestureNameSequenceArray.IsValidIndex(CurrentGestureIndex))
				{
					RightHandCompObject->GetWorld()->GetTimerManager().ClearTimer(CancelGestureSequenceTimer);
					RightHandCompObject->GetWorld()->GetTimerManager().SetTimer(CancelGestureSequenceTimer, this, &UGestureMovingSequenceTask::GestureSequenceCancelled, TimerInterval, false, TimerInterval);
				}
				else
				{
					RightHandCompObject->GetWorld()->GetTimerManager().ClearTimer(CancelGestureSequenceTimer);
					CurrentGestureIndex = 0;
				}
			}
		}
	}


}

void UGestureMovingSequenceTask::GestureSequenceCancelled()
{
	LeftGestureActivated = false;
	RightGestureActivated = false;
	CurrentGestureIndex = 0;
	OnGestureSequenceCancelled.Broadcast(-1);

	if (LeftHandCompObject)
	{
		LeftHandCompObject->GetWorld()->GetTimerManager().ClearTimer(CancelGestureSequenceTimer);
	}
	else if (HandCompObject)
	{
		HandCompObject->GetWorld()->GetTimerManager().ClearTimer(CancelGestureSequenceTimer);
	}

}


