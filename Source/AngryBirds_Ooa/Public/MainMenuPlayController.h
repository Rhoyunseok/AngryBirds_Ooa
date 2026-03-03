// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MainMenuPlayController.generated.h"

/**
 * 
 */
UCLASS()
class ANGRYBIRDS_OOA_API AMainMenuPlayController : public APlayerController
{
	GENERATED_BODY()
protected:
	virtual void BeginPlay() override;
	
public:
	// 블루프린트에서 메인 메뉴 위젯 클래스를 지정할 수 있도록 하는 변수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="UI")
	// TSubclassOf는 특정 클래스의 서브클래스만 허용하는 템플릿입니다. 여기서는 UUserWidget의 서브클래스만 지정할 수 있도록 합니다.
	TSubclassOf<UUserWidget> MainMenuWidgetClass; // 메인 메뉴 위

private:
	UPROPERTY()
	class UUserWidget* MainMenuWidget; // 실제로 생성된 위젯 인스
};
