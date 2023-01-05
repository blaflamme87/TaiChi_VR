// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomHandComponent.h"
#include "TimerManager.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"

UCustomHandComponent::UCustomHandComponent()
{

}

void UCustomHandComponent::BeginPlay()
{
	Super::BeginPlay();

	switch (GestureLoadSource)
	{
	case E_GestureLoadSource::FromDataTable:
		Gestures = LoadGesturesFromDataTable();
		break;

	case E_GestureLoadSource::FromSaveGameObject:
		Gestures = LoadGesturesSaveObject()->Gestures;
		break;

	default: break;
	}

	GetWorld()->GetTimerManager().SetTimer(DetectGesturesTimerHandle, this, &UCustomHandComponent::DetectGesturesOnTimer, GestureDetectionRefreshRate, true, 0.f);
	GetWorld()->GetTimerManager().SetTimer(UpdateGrabAndPointerTimerHandle, this, &UCustomHandComponent::UpdateGrabAndPointerOnTimer, GrabAndPointerRefreshRate, true, 0.f);
}

void UCustomHandComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}


UGestureSaveGame* UCustomHandComponent::LoadGesturesSaveObject()
{
	bool bSaveGameExists = UGameplayStatics::DoesSaveGameExist(GestureGameSlotName, 0);

	if (bSaveGameExists)
	{
		UGestureSaveGame* GestureGameSave = Cast<UGestureSaveGame>(UGameplayStatics::LoadGameFromSlot(GestureGameSlotName, 0));

		if (GestureGameSave)
		{
			return GestureGameSave;
		}
	}
	else
	{
		//UE_LOG(LogTemp, Warning, TEXT("Tezt no save game, making one now"));
		return Cast<UGestureSaveGame>(UGameplayStatics::CreateSaveGameObject(UGestureSaveGame::StaticClass()));
	}

	//UE_LOG(LogTemp, Warning, TEXT("Tezt None of that worked, returning nullptr"));
	return nullptr;
}

TArray<FGesture> UCustomHandComponent::LoadGesturesFromDataTable()
{
	TArray<FGesture> LocalGestures;
	UDataTable* CurrentDataTable = nullptr;

	switch (SkeletonType)
	{
	case EOculusHandType::HandLeft:
		CurrentDataTable = GesturesLeftDataTable;
		break;

	case EOculusHandType::HandRight:
		CurrentDataTable = GesturesRightDataTable;
		break;
	}

	if (CurrentDataTable)
	{
		for (FName RowName : CurrentDataTable->GetRowNames())
		{
			static const FString ContextString(TEXT("Gesture Data Context"));
			FGestureData* LocalGestureData = CurrentDataTable->FindRow<FGestureData>(RowName, ContextString, true);

			if (!LocalGestureData)
			{
				UE_LOG(LogTemp, Warning, TEXT("tezt the gesture data is null"));
				return LocalGestures;
			}
			FGesture LocalGesture;
			LocalGesture.BoneLocations = LocalGestureData->BoneLocations;
			LocalGesture.GestureName = RowName;
			LocalGesture.RootTransformMatters = LocalGestureData->RootTransformMatters;
			LocalGesture.RootTransform = FTransform(LocalGestureData->RootRotation, LocalGestureData->RootLocation, FVector(1.f, 1.f, 1.f));

			LocalGestures.Add(LocalGesture);
		}
	}
	return LocalGestures;
}

bool UCustomHandComponent::DoesGestureExist(UGestureSaveGame* SaveObject, FName GestureName)
{

	for (FGesture Gesture : Gestures)
	{
		if (GestureName == Gesture.GestureName)
		{
			return true;
		}
	}

	return false;
}

void UCustomHandComponent::RecordGesture(FName GestureName, bool RootTransformMatters)
{
	GestureSaveGameObject = LoadGesturesSaveObject();


	TArray<FVector> BoneLocations;
	FGesture Gesture;

	if (RootTransformMatters)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Root Transform Matters in record gesture"));

		APlayerCameraManager* CamManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);

		FRotator AdjustedCamRotation = FRotator(0.f, CamManager->GetCameraRotation().Yaw, 0.f);
		FTransform AdjustedCameraTransform = FTransform(AdjustedCamRotation, CamManager->GetCameraLocation(), FVector(1.f, 1.f, 1.f));
		FTransform LocalComponentTransform = GetComponentTransform().GetRelativeTransform(AdjustedCameraTransform);


		Gesture.RootTransform = LocalComponentTransform;
		Gesture.RootTransformMatters = true;
	}

	int LastBoneIndex = GetNumBones() - 1;
	for (int i = 0; i < LastBoneIndex; i++)
	{
		FName BoneName = GetBoneName(i);
		FVector BoneLocation = GetBoneLocationByName(BoneName, EBoneSpaces::ComponentSpace);
		BoneLocations.Add(BoneLocation);
	}

	Gesture.BoneLocations = BoneLocations;
	Gesture.GestureName = GestureName;
	GestureSaveGameObject->Gestures.Add(Gesture);

	if (UGameplayStatics::SaveGameToSlot(GestureSaveGameObject, GestureGameSlotName, 0))
	{
		UE_LOG(LogTemp, Warning, TEXT("Tezt Save game Succeeded"));
	}

}

FGesture UCustomHandComponent::RecordGestureToDisk(FName GestureName)
{

	FGesture Gesture;


	TArray<FVector> BoneLocations;

	int LastBoneIndex = GetNumBones() - 1;

	for (int i = 0; i < LastBoneIndex; i++)
	{
		FName BoneName = GetBoneName(i);
		FVector BoneLocation = GetBoneLocationByName(BoneName, EBoneSpaces::ComponentSpace);
		BoneLocations.Add(BoneLocation);
		UE_LOG(LogTemp, Warning, TEXT("turzt Write Gesture : Bone Location is %s"), *BoneLocation.ToString());
	}


	Gesture.BoneLocations = BoneLocations;
	Gesture.GestureName = GestureName;

	return Gesture;


}

void UCustomHandComponent::DeleteGesture(FName GestureName)
{
	GestureSaveGameObject = LoadGesturesSaveObject();
	if (!GestureSaveGameObject) { return; }

	int LastIndex = GestureSaveGameObject->Gestures.Num() - 1;

	for (int i = 0; i < LastIndex; i++)
	{
		FName GestureNameAtIndex = GestureSaveGameObject->Gestures[i].GestureName;

		if (GestureNameAtIndex == GestureName)
		{
			GestureSaveGameObject->Gestures.RemoveAt(i);
		}
	}

	UGameplayStatics::SaveGameToSlot(GestureSaveGameObject, GestureGameSlotName, 0);

}

TArray<FGesture> UCustomHandComponent::DetectCurrentGestures()
{
	TArray<FGesture> ValidGestures;

	for (FGesture Gesture : Gestures)
	{

		bool bValidGesture = true;

		if (Gesture.RootTransformMatters)
		{
			APlayerCameraManager* CamManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
			FRotator AdjustedCamRotation = FRotator(0.f, CamManager->GetCameraRotation().Yaw, 0.f);
			FTransform AdjustedCameraTransform = FTransform(AdjustedCamRotation, CamManager->GetCameraLocation(), FVector(1.f, 1.f, 1.f));
			FTransform LocalComponentTransform = GetComponentTransform().GetRelativeTransform(AdjustedCameraTransform);
			bool LocationMatch = FTransform::AreTranslationsEqual(Gesture.RootTransform, LocalComponentTransform, GestureRootLocationThreshold);

			FVector GestureWorldUpVector = AdjustedCameraTransform.TransformRotation(Gesture.RootTransform.GetRotation()).GetUpVector();
			FVector ComponentWorldUpVector = GetComponentRotation().Quaternion().GetUpVector();
			//DrawDebugLine(GetWorld(), GetComponentLocation(), GetComponentLocation() + (ComponentWorldUpVector * 25.f), FColor::Green, false, .033f);
			//DrawDebugLine(GetWorld(), GetBoneLocation(GetBoneName(0)), GetBoneLocation(GetBoneName(0)) + (GestureWorldUpVector * 25.f), FColor::Red, false, .033f);
			bool RotationMatch = GestureWorldUpVector.Dot(ComponentWorldUpVector) > GestureRootRotationDotThreshold;

			if (!(LocationMatch && RotationMatch)) { continue; }

		}

		int LastIndex = Gesture.BoneLocations.Num();
		for (int i = 0; i < LastIndex; i++)
		{
			FVector GestureBoneLocationAtIndex = Gesture.BoneLocations[i];
			FVector CurrentBoneLocationAtIndex = GetBoneLocationByName(GetBoneName(i), EBoneSpaces::ComponentSpace);



			float BoneLocationDifference = FMath::Abs((GestureBoneLocationAtIndex - CurrentBoneLocationAtIndex).Size());

			if (BoneLocationDifference > GestureThreshold)
			{
				bValidGesture = false;
			}
		}

		if (bValidGesture)
		{
			ValidGestures.Add(Gesture);
		}
	}
	return ValidGestures;
}



FGesture UCustomHandComponent::GetGestureFromName(FName GestureName) const
{
	for (FGesture Gesture : Gestures)
	{
		if (Gesture.GestureName == GestureName)
		{
			return Gesture;
		}
	}

	return FGesture();
}

void UCustomHandComponent::DetectGesturesOnTimer()
{
	TArray<FGesture> NewCurrentGestures = DetectCurrentGestures();


	if (!CurrentActiveGestures.IsValidIndex(0))
	{

		if (NewCurrentGestures.Num() < 1)
		{
			CurrentActiveGestures = NewCurrentGestures;

			TArray<FName> GestureNames;
			for (FGesture Gesture : CurrentActiveGestures)
			{
				GestureNames.Add(Gesture.GestureName);
			}

			ActiveGesturesUsed.Broadcast(GestureNames);
		}
	}
	else
	{
		if (NewCurrentGestures[0].GestureName == CurrentActiveGestures[0].GestureName)
		{
			CurrentLeewayTimeElapsed = 0.f;
		}
		else
		{
			if (CurrentLeewayTimeElapsed < GestureLeewayThreshold)
			{
				CurrentLeewayTimeElapsed += GestureDetectionRefreshRate;
			}
			else
			{
				CurrentActiveGestures = NewCurrentGestures;

				TArray<FName> GestureNames;
				for (FGesture Gesture : CurrentActiveGestures)
				{
					GestureNames.Add(Gesture.GestureName);
				}

				ActiveGesturesUsed.Broadcast(GestureNames);
			}
		}
	}

	CurrentGestures = NewCurrentGestures;

	TArray<FName> GestureNames;
	for (FGesture Gesture : CurrentGestures)
	{
		GestureNames.Add(Gesture.GestureName);
	}

	OnGesturesDetected.Broadcast(GestureNames);
}

void UCustomHandComponent::UpdateGrabAndPointerOnTimer()
{
	bool bValidReading = SetGrabAndPointerAxes();

	if (bValidReading)
	{
		OnGrabAxis.Broadcast(GrabAxis);

		if (GrabAxis > GrabThreshold)
		{
			if (!bIsGrabbing)
			{
				bIsGrabbing = true;
				OnGrab.Broadcast(E_GrabState::Grabbed);
			}
		}
		else
		{
			if (bIsGrabbing)
			{
				bIsGrabbing = false;
				OnGrab.Broadcast(E_GrabState::Dropped);
			}
		}

		OnPullTriggerAxis.Broadcast(PullTriggerAxis);

		if (PullTriggerAxis > PullTriggerThreshold)
		{
			if (!bIsPullingTrigger)
			{
				bIsPullingTrigger = true;
				OnPullTrigger.Broadcast(E_TriggerState::Pulled);
			}
		}
		else
		{
			if (bIsPullingTrigger)
			{
				bIsPullingTrigger = false;
				OnPullTrigger.Broadcast(E_TriggerState::Released);
			}
		}

	}
}

bool UCustomHandComponent::SetGrabAndPointerAxes()
{

	bool bValidHandReading = UOculusInputFunctionLibrary::IsHandTrackingEnabled() && UOculusInputFunctionLibrary::IsHandPositionValid(SkeletonType);
	if (!bValidHandReading) { return false; }

	FVector WristRoot = GetBoneLocation(GetBoneName(0));
	float CombinedFingertipDistance = 0.f;
	int NumFingertips = 0;

	int LastBoneIndex = GetNumBones() - 1;
	for (int i = 0; i < LastBoneIndex; i++)
	{
		FString BoneNameString = GetBoneName(i).ToString();
		bool bContainsTip = BoneNameString.Contains(FingertipString);

		if (bContainsTip)
		{
			if (BoneNameString.Contains(IndexFingerString))
			{
				FVector IndexTipLocation = GetBoneLocation(GetBoneName(i));
				float AbsoluteDistance = FMath::Abs((IndexTipLocation - WristRoot).Length());
				PullTriggerAxis = FMath::GetMappedRangeValueClamped(FingerDistRange, FFloatRange(0.f, 1.f), AbsoluteDistance);
			}
			else
			{
				CombinedFingertipDistance += FMath::Abs((GetBoneLocation(GetBoneName(i)) - WristRoot).Length());
				NumFingertips++;
			}
		}
	}

	if (CombinedFingertipDistance <= 0) { return false; }

	float AverageFingertipDistance = CombinedFingertipDistance / NumFingertips;
	GrabAxis = FMath::GetMappedRangeValueClamped(FingerDistRange, FFloatRange(0.f, 1.f), AverageFingertipDistance);

	return true;
}
