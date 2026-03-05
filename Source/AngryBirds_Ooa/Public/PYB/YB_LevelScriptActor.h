// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/LevelScriptActor.h"
#include "YB_LevelScriptActor.generated.h"

/**
 * 
 */
UCLASS()
class ANGRYBIRDS_OOA_API AYB_LevelScriptActor : public ALevelScriptActor
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;
	
public:
	UPROPERTY(BlueprintReadWrite)
	int32 PigNum;
	UPROPERTY(EditAnywhere)
	FString StageInfo;
	
	UFUNCTION()
	void ShowLevelInfo();
};
