// Fill out your copyright notice in the Description page of Project Settings.


#include "GestureTutorial.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AGestureTutorial::AGestureTutorial()
{

	Root = CreateDefaultSubobject<USceneComponent>("Root");
	SetRootComponent(Root);

	TutorialHandLeft = CreateDefaultSubobject<UPoseableMeshComponent>("TutorialHandLeft");
	TutorialHandLeft->SetupAttachment(Root);

	TutorialHandRight = CreateDefaultSubobject<UPoseableMeshComponent>("TutorialHandRight");
	TutorialHandRight->SetupAttachment(Root);

 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AGestureTutorial::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGestureTutorial::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AGestureTutorial::StartGestureSequenceTutorial(UCustomHandComponent* CustomHandLeft, UCustomHandComponent* CustomHandRight, TArray<FName> SequenceGestureNames)
{
	APlayerCameraManager* CamManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	FRotator AdjustedCamRotation = FRotator(0.f, CamManager->GetCameraRotation().Yaw, 0.f);
	OriginalCamTransform = FTransform(AdjustedCamRotation, CamManager->GetCameraLocation(), FVector(1.f, 1.f, 1.f));

	CurrentHandLeft = CustomHandLeft;
	CurrentHandRight = CustomHandRight;
	GestureNames = SequenceGestureNames;

	CurrentGestureIndex = 0;

	if (GestureNames.IsValidIndex(0))
	{
		TrackGesture(GestureNames[0]);
		GetWorld()->GetTimerManager().SetTimer(DetectHandPoseUpdateTimer, this, &AGestureTutorial::CheckHandsUpdate, DetectHandPoseUpdateInterval, true, 0.f);
	}
}

void AGestureTutorial::TrackGesture(FName GestureName)
{
	if (!(CurrentHandLeft && CurrentHandRight)) 
	{
		return; 
	}

	GestureLeft = CurrentHandLeft->GetGestureFromName(GestureName);
	GestureRight = CurrentHandRight->GetGestureFromName(GestureName);

	if (TutorialHandLeft)
	{
		UE_LOG(LogTemp, Warning, TEXT("Tezt We have a LEFT hand!"))
	}
	if (TutorialHandRight)
	{
		UE_LOG(LogTemp, Warning, TEXT("Tezt We have a LEFT hand!"))
	}
	PoseHand(TutorialHandLeft, GestureLeft);
	PoseHand(TutorialHandRight, GestureRight);

	GetWorld()->GetTimerManager().ClearTimer(HandResetTimer);
	GetWorld()->GetTimerManager().SetTimer(HandResetTimer, this, &AGestureTutorial::ResetTimerExpired, HandDetectionExpiredTime, false, HandDetectionExpiredTime);
	


}

void AGestureTutorial::PoseHand(UPoseableMeshComponent* HandToPose, FGesture GestureData)
{
	if (!HandToPose)
	{ 
		UE_LOG(LogTemp, Warning, TEXT("Tezt NO HAND TO POSE!"))

		return; 
	}

	//APlayerCameraManager* CamManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	//FRotator AdjustedCamRotation = FRotator(0.f, CamManager->GetCameraRotation().Yaw, 0.f);
	//FTransform AdjustedCamTransform = FTransform(AdjustedCamRotation, CamManager->GetCameraLocation(), FVector(1.f, 1.f, 1.f));
	FVector WorldGestureRootLocation = OriginalCamTransform.TransformPosition(GestureData.RootTransform.GetLocation());
	FRotator WorldGestureRootRotation = FRotator(OriginalCamTransform.TransformRotation(GestureData.RootTransform.GetRotation()));
	FTransform TutorialHandWorldTransform = FTransform(WorldGestureRootRotation, WorldGestureRootLocation, FVector(1.f, 1.f, 1.f));
	HandToPose->SetWorldTransform(TutorialHandWorldTransform);

	for (auto Element : GestureData.BoneTransformMap)
	{
		FName BoneName = Element.Key;
		FQuat BoneRotation = Element.Value.GetRotation();

		HandToPose->SetBoneRotationByName(BoneName, FRotator(BoneRotation),EBoneSpaces::ComponentSpace);
	}
}

void AGestureTutorial::CheckHandsUpdate()
{
	bool LeftHandMatch = CheckHand(TutorialHandLeft, CurrentHandLeft);
	bool RightHandMatch = CheckHand(TutorialHandRight, CurrentHandRight);
	
	if (LeftHandMatch && RightHandMatch)
	{
		//UE_LOG(LogTemp, Warning, TEXT("we are matching the pose! tezt"));

		OnNextGestureDetected.Broadcast(CurrentGestureIndex);

		CurrentGestureIndex++;

		if (GestureNames.IsValidIndex(CurrentGestureIndex))
		{
			TrackGesture(GestureNames[CurrentGestureIndex]);
		}
		else
		{
			SequenceCompleted();
		}
	}
}

bool AGestureTutorial::CheckHand(UPoseableMeshComponent* HandToMatch, UCustomHandComponent* HandToCheck)
{
	if (!(HandToMatch && HandToCheck)) { return false; }

	bool bHandMatchesPose = true;

	TArray<FName> BoneNames;
	HandToMatch->GetBoneNames(BoneNames);
	for (FName BoneName : BoneNames)
	{
		UE_LOG(LogTemp, Warning, TEXT("tezt bone name is %s"), *BoneName.ToString());

		FVector TutorialBoneLocation = HandToMatch->GetBoneLocationByName(BoneName, EBoneSpaces::WorldSpace);
		FVector CurrentBoneLocation = HandToCheck->GetBoneLocationByName(BoneName, EBoneSpaces::WorldSpace);
		float BoneDistanceFromDesiredLocation = FVector::Dist(TutorialBoneLocation, CurrentBoneLocation);

		if (BoneDistanceFromDesiredLocation > BoneLocationThreshold)
		{
			bHandMatchesPose = false;
		}

	}


	return bHandMatchesPose;
}

void AGestureTutorial::ResetTimerExpired()
{	
	GetWorld()->GetTimerManager().ClearTimer(HandResetTimer);
	GetWorld()->GetTimerManager().ClearTimer(DetectHandPoseUpdateTimer);
	OnGestureSequenceCancelled.Broadcast();

}

void AGestureTutorial::SequenceCompleted()
{
	GetWorld()->GetTimerManager().ClearTimer(HandResetTimer);
	GetWorld()->GetTimerManager().ClearTimer(DetectHandPoseUpdateTimer);
	OnGestureSequenceCompleted.Broadcast();
}

