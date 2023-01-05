// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "GestureStruct.h"
#include "GestureSaveGame.generated.h"



/**
 *
 */
UCLASS()
class TAICHIGAME_API UGestureSaveGame : public USaveGame
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FGesture> Gestures;


};