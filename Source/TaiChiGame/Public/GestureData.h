#pragma once
#include "Engine/DataTable.h"
#include "GestureData.generated.h"

USTRUCT(BlueprintType)
struct FGestureData : public FTableRowBase
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FVector> BoneLocations;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool RootTransformMatters;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FRotator RootRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector RootLocation;

};