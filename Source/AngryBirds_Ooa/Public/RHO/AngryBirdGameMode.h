// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "AngryBirdGameMode.generated.h"

/**
 * 
 */
UCLASS()
class ANGRYBIRDS_OOA_API AAngryBirdGameMode : public AGameModeBase
{
	GENERATED_BODY()
public:
	AAngryBirdGameMode();
	
	// 블루프린트에서 레벨(맵)마다 띄울 메인 위젯을 할당할 클래스 변수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<class UUserWidget> WidgetClassName;
	
protected:
	virtual void BeginPlay() override;

private:
	// 화면에 띄운 위젯을 기억해둘 포인터 (나중에 참조하거나 지울 때 유용함)
	UPROPERTY()
	class UUserWidget* CurrentWidget;
};
