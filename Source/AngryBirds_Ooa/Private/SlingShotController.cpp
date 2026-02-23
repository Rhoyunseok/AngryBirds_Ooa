// Fill out your copyright notice in the Description page of Project Settings.


#include "SlingShotController.h"
#include "SlingShot.h" // 새총 헤더 포함
#include "Kismet/GameplayStatics.h"

ASlingShotController::ASlingShotController()
{
	// 마우스 커서 보이게 설정
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Crosshairs; //커서 모양 변경
}

void ASlingShotController::BeginPlay()
{
	Super::BeginPlay();

	// 맵에 있는 새총 액터를 찾아서 CurrentSlingshot 변수에 저장합니다.
	AActor* FoundActor = UGameplayStatics::GetActorOfClass(GetWorld(), ASlingShot::StaticClass());
	if (FoundActor)
	{
		// 맵에 새총이 하나 이상 있을 경우, 첫 번째로 발견된 새총을 사용합니다.
		CurrentSlingshot = Cast<ASlingShot>(FoundActor);
	}
}

// 입력 바인딩
void ASlingShotController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	// "Fire" 액션은 마우스 왼쪽 버튼에 바인딩되어 있다고 가정합니다. (프로젝트 세팅에서 설정 필요)
	// "MouseWheel" 축은 마우스 휠에 바인딩되어 있다고 가정합니다. (프로젝트 세팅에서 설정 필요)
	if (InputComponent)
	{
		InputComponent->BindAction("Fire", IE_Pressed, this, &ASlingShotController::StartAiming);
		InputComponent->BindAction("Fire", IE_Released, this, &ASlingShotController::StopAiming);
		InputComponent->BindAxis("MouseWheel", this, &ASlingShotController::AdjustPower);
	}
}

// 조준 시작 함수: 마우스 왼쪽 버튼을 누르면 새총이 조준 모드로 들어갑니다.
void ASlingShotController::StartAiming()
{
	if (CurrentSlingshot)
	{
		
		CurrentSlingshot->bIsAiming = true;
	}
}

// 마우스 휠을 굴려서 파워 조절하는 함수: 휠을 굴릴 때마다 새총의 PullPower 변수를 증가/감
// AxisValue 를 음수로 설정(프로젝트 셋팅) 하여야 당기면 강해지고 밀면 약해진다.
void ASlingShotController::AdjustPower(float AxisValue)
{
	if (CurrentSlingshot && CurrentSlingshot->bIsAiming)
	{
		// 휠을 굴릴 때마다 파워 증감 (민감도 조절 가능)
		float NewPower = CurrentSlingshot->PullPower + (AxisValue * 5.0f);
		CurrentSlingshot->PullPower = FMath::Clamp(NewPower, 0.0f, 300.0f);
	}
}


void ASlingShotController::StopAiming()
{
	if (CurrentSlingshot)
	{
		CurrentSlingshot->bIsAiming = false;
		CurrentSlingshot->bIsReturning = true;
	}
}