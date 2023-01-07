#pragma once
#include "BoneTransformData.generated.h"


USTRUCT(BlueprintType)
struct FBoneTransformData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FName, FTransform> BoneTransformMap;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BoneLocationTolerance = 3.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BoneRotationTolerance = 45.f;

};
