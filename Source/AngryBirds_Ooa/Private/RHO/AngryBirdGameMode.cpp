// Fill out your copyright notice in the Description page of Project Settings.


#include "RHO/AngryBirdGameMode.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h" // UGameplayStatics::GetPlayerController() 사용하기 위해

AAngryBirdGameMode::AAngryBirdGameMode()
{
	
}

void AAngryBirdGameMode::BeginPlay()
{
	Super::BeginPlay();

	// 1. 플레이어 컨트롤러를 가져와서 마우스와 입력 모드 설정
	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		// 앵그리버드는 메뉴든 게임이든 마우스(터치)가 필수니까 켜줌
		PC->bShowMouseCursor = true; 

		// UI 버튼도 누르고, 인게임에서 새총(SlingShot)도 당겨야 하므로 GameAndUI 모드 사용
		FInputModeGameAndUI InputMode;
		// 클릭했을 때 마우스가 화면 밖으로 나가지 않게 하려면 아래 옵션 추가 (선택사항)
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		PC->SetInputMode(InputMode);
	}

	// 2. 블루프린트에서 할당한 위젯이 있다면 화면에 생성하고 띄우기
	if (WidgetClassName != nullptr)
	{
		// 헤더에 선언해둔 내 멤버 변수(CurrentWidget)에 그대로 담아줌
		CurrentWidget = CreateWidget<UUserWidget>(GetWorld(), WidgetClassName);
		if (CurrentWidget != nullptr)
		{
			CurrentWidget->AddToViewport();
		}
	}
}