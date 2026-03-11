// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/LevelScriptActor.h"
#include "Stage1_1_LevelScriptActor.generated.h"

/**
 * 
 */
UCLASS()
class ANGRYBIRDS_OOA_API AStage1_1_LevelScriptActor : public ALevelScriptActor
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;
	
public:
	UPROPERTY(BlueprintReadWrite)
	int32 PigNum;
	
	UPROPERTY(EditAnywhere)
	FString StageInfo;
	
	//새
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Settings")
	TArray<TSubclassOf<class AActor>> LevelBirds;
	
	UFUNCTION()
	void ShowLevelInfo();
	UFUNCTION()
	void OnGameSuccess();
	UFUNCTION()
	void OnGameFail();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI Settings")
	TSubclassOf<class UUserWidget> SuccessWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI Settings")
	TSubclassOf<class UUserWidget> FailWidgetClass;
};

