#pragma once
#include "GestureStruct.generated.h"

USTRUCT(BlueprintType)
struct FGesture
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName GestureName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FVector> BoneLocations;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool RootTransformMatters = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTransform RootTransform;

};