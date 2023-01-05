#pragma once
#include "BoneData.generated.h"


USTRUCT(BlueprintType)
struct FBoneData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName BoneName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector BoneLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator BoneRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BoneLocationTolerance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BoneRotationTolerance;

};
