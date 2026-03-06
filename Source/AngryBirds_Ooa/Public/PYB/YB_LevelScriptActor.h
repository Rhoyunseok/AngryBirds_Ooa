// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/LevelScriptActor.h"
#include "YB_LevelScriptActor.generated.h"

// gamestate 에 돼지의 수를 전달하는 역할을 하는 레벨 스크립트 액터입니다.
// BeginPlay에서 돼지의 수를 GameState에 전달하는 방식으로 구현할 예정입니다. (코드 위치는 YB_LevelScriptActor.cpp 참고)
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
	
	//새
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Settings")
	TArray<TSubclassOf<class AActor>> LevelBirds;
	
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI Settings")
	TSubclassOf<class UUserWidget> SuccessWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI Settings")
	TSubclassOf<class UUserWidget> FailWidgetClass;
	
	
	UFUNCTION()
	void ShowLevelInfo();
	UFUNCTION()
	void OnGameSuccess();

	UFUNCTION()
	void OnGameFail();
};
