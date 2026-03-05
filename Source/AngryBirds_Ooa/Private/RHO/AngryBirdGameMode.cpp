// Fill out your copyright notice in the Description page of Project Settings.


#include "RHO/AngryBirdGameMode.h"

#include "SlingShotController.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h" // UGameplayStatics::GetPlayerController() 사용하기 위해

AAngryBirdGameMode::AAngryBirdGameMode()
{
	PlayerControllerClass = ASlingShotController::StaticClass();
}

void AAngryBirdGameMode::BeginPlay()
{
	Super::BeginPlay();

	// 1. 플레이어 컨트롤러를 가져와서 마우스와 입력 모드 설정
	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		PC->bShowMouseCursor = true; 
		FInputModeGameAndUI InputMode;
       
		// 드래그 시 마우스 포커스를 잃지 않게 하려면 아래 줄을 추가하는 것도 좋습니다.
		InputMode.SetHideCursorDuringCapture(false); 
       
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		PC->SetInputMode(InputMode);
	}

	// StagePlayingWidget 클래스가 블루프린트에서 할당되어 있다면, 화면에 띄워줍니다.
	if (WidgetClassName)
	{
		CurrentWidget = CreateWidget<UUserWidget>(GetWorld(), WidgetClassName);
		if (CurrentWidget)
		{			CurrentWidget->AddToViewport();
		}
	}
}