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
//	FInputModeGameAndUI InputMode;
//	InputMode.SetHideCursorDuringCapture(false); // 드래그 중 커서 숨김 방지
	//InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
    
//	this->SetInputMode(InputMode); // 컨트롤러 자신에게 입력 모드 적용f
}

// 입력 바인딩
void ASlingShotController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	// "Fire" 액션은 마우스 왼쪽 버튼에 바인딩되어 있다고 가정합니다. (프로젝트 세팅에서 설정 필요)
	// "MouseWheel" 축은 마우스 휠에 바인딩되어 있다고 가정합니다. (프로젝트 세팅에서 설정 필요)
	if (InputComponent)
	{
		// 클릭을 했을때 ui 상호작용이 가능하도록 bConsumeInput = false로 설정하여 입력이 UI에도 전달되도록 합니다.
		InputComponent->BindAction("Fire", IE_Pressed, this, &ASlingShotController::StartAiming).bConsumeInput = false;
		InputComponent->BindAction("Fire", IE_Released, this, &ASlingShotController::StopAiming).bConsumeInput = false;
		InputComponent->BindAxis("MouseWheel", this, &ASlingShotController::AdjustPower);
		InputComponent->BindAction("UseAbility", IE_Pressed, this, &ASlingShotController::OnUseAbilityPressed);
	}
}

// 조준 시작 함수: 마우스 왼쪽 버튼을 누르면 새총이 조준 모드로 들어갑니다.
void ASlingShotController::StartAiming()
{
	if (CurrentSlingshot)
	{
		CurrentSlingshot->PullString();
	}
}

// 마우스 휠을 굴려서 파워 조절하는 함수: 휠을 굴릴 때마다 새총의 PullPower 변수를 증가/감
// AxisValue 를 음수로 설정(프로젝트 셋팅) 하여야 당기면 강해지고 밀면 약해진다.
void ASlingShotController::AdjustPower(float AxisValue)
{
	if (CurrentSlingshot && CurrentSlingshot->bIsAiming)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Cyan, TEXT("Mouse Wheel Moving!"));
		// 휠을 굴릴 때마다 파워 증감 (민감도 조절 가능)
		float NewPower = CurrentSlingshot->PullPower + (AxisValue * 30.0f);
		CurrentSlingshot->PullPower = FMath::Clamp(NewPower, 0.0f, 700.0f);
		// 로그를 띄워 실시간으로 수치가 변하는지 확인하세요.
		// GEngine->AddOnScreenDebugMessage(1, 1.0f, FColor::Yellow, 
			// FString::Printf(TEXT("Current PullPower: %f"), CurrentSlingshot->PullPower));
	}
}


void ASlingShotController::StopAiming()
{
	if (CurrentSlingshot)
	{
		// 1. 조준 상태를 해제하고
		CurrentSlingshot->bIsAiming = false;
       
		// 2. ★ 중요 ★ 새총에 작성된 '발사 로직'을 실행시킵니다!
		CurrentSlingshot->ReleaseString(); 
       
		// 3. 파우치가 돌아오는 애니메이션을 위해 설정
		CurrentSlingshot->bIsReturning = true;
	}
}
void ASlingShotController::OnUseAbilityPressed()
{
	if (CurrentSlingshot)
	{
		CurrentSlingshot->TriggerBirdAbility();
	}
}