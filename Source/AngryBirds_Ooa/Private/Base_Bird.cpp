#include "Base_Bird.h"

#include "GameFramework/ProjectileMovementComponent.h"

#include "Components/SkeletalMeshComponent.h"

#include "GameFramework/SpringArmComponent.h"

#include "Camera/CameraComponent.h"

#include "GameFramework/PlayerController.h"

#include "Framework/Application/NavigationConfig.h"

#include "Engine/EngineTypes.h"



ABase_Bird::ABase_Bird()

{

PrimaryActorTick.bCanEverTick = true;



// 1. Root 설정

RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));

RootComponent = RootScene;



// 2. 메쉬 설정

BirdMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BirdMesh"));

BirdMesh->SetupAttachment(RootScene);

BirdMesh->SetSimulatePhysics(true);

BirdMesh->SetCollisionProfileName(TEXT("PhysicsActor"));



// 3. 스프링 암 설정 (카메라 추적의 핵심)

SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));

SpringArm->SetupAttachment(BirdMesh); // 메쉬를 따라다니도록 부착

SpringArm->TargetArmLength = 800.0f;

// ★ 핵심 수정: 새가 회전해도 카메라는 돌지 않도록 설정

SpringArm->bInheritPitch = false;

SpringArm->bInheritYaw = false;

SpringArm->bInheritRoll = false;



// ★ 카메라 래그: 카메라가 새를 더 부드럽게 쫓아오게 함

SpringArm->bEnableCameraLag = true;

SpringArm->CameraLagSpeed = 3.0f; // 낮을수록 더 부드럽게 따라옴



// 4. 카메라 설정

FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));

FollowCamera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);



// 5. 무브먼트 설정

ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));

ProjectileMovement->UpdatedComponent = BirdMesh;

ProjectileMovement->bAutoActivate = false;

ProjectileMovement->MaxSpeed = 10000.f;

}



void ABase_Bird::BeginPlay()

{

Super::BeginPlay();

APlayerController* PC = Cast<APlayerController>(GetWorld()->GetFirstPlayerController());

if (PC) {

EnableInput(PC);

PC->bShowMouseCursor = true;

FInputModeGameAndUI InputMode;

PC->SetInputMode(InputMode);

}

}



void ABase_Bird::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)

{

Super::SetupPlayerInputComponent(PlayerInputComponent);

PlayerInputComponent->BindAction("DragShoot", IE_Pressed, this, &ABase_Bird::OnDragStart);

PlayerInputComponent->BindAction("DragShoot", IE_Released, this, &ABase_Bird::OnDragRelease);

}



void ABase_Bird::OnDragStart()

{

APlayerController* PC = Cast<APlayerController>(GetWorld()->GetFirstPlayerController());

// GetMousePosition이 드래그 도중에도 정확히 작동하도록 보장

if (PC && PC->GetMousePosition(StartMousePos.X, StartMousePos.Y))

{

if (StartMousePos.IsZero()) return;



bIsDragging = true;

InitialLocation = GetActorLocation();

// 드래그 중에는 물리를 끄고 수동 이동

BirdMesh->SetSimulatePhysics(false);

BirdMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision); // 충돌도 잠시 끔



// 마우스를 뷰포트에 가두되 커서는 유지

FInputModeGameAndUI InputMode;

// 아래 두 줄은 기본값이므로 에러가 나면 과감히 지우셔도 됩니다.

// PC->SetInputMode만 제대로 호출되면 엔진 기본 설정이 적용됩니다.

PC->SetInputMode(InputMode);

}

}



void ABase_Bird::OnDragRelease()
{
	if (!bIsDragging) return;
	bIsDragging = false;

	APlayerController* PC = Cast<APlayerController>(GetWorld()->GetFirstPlayerController());
	FVector2D CurrentMousePos;
    
	if (PC && PC->GetMousePosition(CurrentMousePos.X, CurrentMousePos.Y))
	{
		FVector2D DragDelta = CurrentMousePos - StartMousePos;
        
		if (DragDelta.Size() > 20.0f) {
			// 1. 기본 조준 방향 (Yaw)
			float YawAngle = FMath::Clamp((-DragDelta.X / MaxDragDist) * 45.0f, -45.0f, 45.0f);

			// 2. 고도(Pitch) 로직 강화: (상하 절대값 - 좌우 절대값) 기반
			// 상하 드래그가 지배적일수록 고도가 급격히 상승합니다.
			float VerticalBias = (FMath::Abs(DragDelta.Y) - FMath::Abs(DragDelta.X)) / MaxDragDist;
            
			// 0에서 90도까지 매핑 (VerticalBias가 1에 가까울수록 90도에 수렴)
			// 지면 아래로 쏘는 것을 방지하기 위해 0~90도 사이로 Clamp
			float PitchAngle = FMath::Clamp(VerticalBias * 90.0f, 0.0f, 90.0f);

			// 3. 발사 벡터 생성
			// Rotation 순서상 Pitch가 90도면 하늘 정면을 향합니다.
			FVector LaunchDir = FRotator(PitchAngle, YawAngle, 0.f).Vector();
            
			// 힘(Power)은 전체 드래그 길이에 비례
			float Power = FMath::Clamp(DragDelta.Size() / MaxDragDist, 0.0f, 1.0f) * 2500.0f;

			LaunchByVector(LaunchDir * Power);
		}
		else {
			SetActorLocation(InitialLocation);
			BirdMesh->SetSimulatePhysics(true);
		}
	}
}


void ABase_Bird::LaunchByVector(FVector LaunchVelocity)
{
	if (!BirdMesh) return;

	// 발사 방향으로 초기 각도 설정
	SetActorRotation(LaunchVelocity.Rotation());

	BirdMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	BirdMesh->SetSimulatePhysics(true);

	// ★ 핵심: 잠금을 모두 false로 풀어야 포물선 방향으로 고개가 돌아갑니다.
	BirdMesh->BodyInstance.bLockXRotation = false;
	BirdMesh->BodyInstance.bLockYRotation = false;
	BirdMesh->BodyInstance.bLockZRotation = false;
	BirdMesh->BodyInstance.UpdatePhysicsFilterData();

	// 중력 배율을 살짝 높여주면 포물선이 더 예쁘게 나옵니다 (선택 사항)
	// BirdMesh->SetApplyGravity(true);

	BirdMesh->SetAllPhysicsLinearVelocity(LaunchVelocity, false);
	bHasLaunched = true;
}



void ABase_Bird::Launch(float VerticalAngle, float HorizontalAngle, float Power)

{

float VRad = FMath::DegreesToRadians(VerticalAngle);

float HRad = FMath::DegreesToRadians(HorizontalAngle);

FVector Dir(FMath::Cos(VRad) * FMath::Sin(HRad), FMath::Cos(VRad) * FMath::Cos(HRad), FMath::Sin(VRad));

LaunchByVector(Dir.GetSafeNormal() * Power);

}



void ABase_Bird::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsDragging) {
		APlayerController* PC = Cast<APlayerController>(GetWorld()->GetFirstPlayerController());
		FVector2D CurrentMouse;
		if (PC && PC->GetMousePosition(CurrentMouse.X, CurrentMouse.Y)) {
			FVector2D DragDelta = CurrentMouse - StartMousePos;

			// 1. 각도 계산
			float PitchAngle = FMath::Clamp((DragDelta.Y / MaxDragDist) * 30.0f, -30.0f, 30.0f);
			float YawAngle = FMath::Clamp((-DragDelta.X / MaxDragDist) * 30.0f, -30.0f, 30.0f);
            
			// 2. 사용자 요청 고도 보정 (상하 - 좌우 차이)
			float HeightBias = (FMath::Abs(DragDelta.Y) - FMath::Abs(DragDelta.X)) / MaxDragDist;
			float FinalPitch = FMath::Clamp(PitchAngle + (HeightBias * 45.0f), -20.0f, 90.0f);

			// ★ 3. 비주얼 회전 보정 (축 교정 핵심) ★
			// 월드 X에 메쉬 Y가 대응되는 경우:
			// 계산된 FinalPitch(상하)를 Roll(세번째 인자)에 넣고, 
			// 원래 옆을 보고 있으므로 Yaw를 -90도(또는 +90도) 틀어 정면을 맞춥니다.
            
			// 만약 고개가 반대로 꺾인다면 -FinalPitch로 바꿔보세요.
			FRotator VisualRot = FRotator(0.0f, YawAngle - 90.0f, -FinalPitch); 

			if (BirdMesh) {
				BirdMesh->SetWorldRotation(VisualRot);
			}

			// 4. 위치 이동 (당겨지는 효과)
			float StrengthRatio = FMath::Clamp(DragDelta.Size() / MaxDragDist, 0.0f, 1.0f);
			// 실제 발사될 방향 벡터
			FVector LaunchDir = FRotator(FinalPitch, YawAngle, 0.0f).Vector();
			FVector BackwardOffset = LaunchDir * -1.0f * (StrengthRatio * MaxVisualDragDist);
			SetActorLocation(InitialLocation + BackwardOffset);



			// 디버그로 수치 확인 (P가 마이너스면 아래, Y가 마이너스면 왼쪽 조준)

			GEngine->AddOnScreenDebugMessage(0, 0.0f, FColor::Cyan,

		FString::Printf(TEXT("Pitch(상하): %.1f | Yaw(좌우): %.1f"), PitchAngle, YawAngle));

		}

	}
	else if (bHasLaunched)
	{
		// 현재 물리적 이동 속도
		FVector CurrentVelocity = BirdMesh->GetPhysicsLinearVelocity();

		// 어느 정도 속도가 있을 때만 방향을 바꿉니다.
		if (CurrentVelocity.Size() > 100.0f) 
		{
			// 1. 이동 방향을 나타내는 각도 추출
			FRotator TargetRot = CurrentVelocity.Rotation();

			// 2. 사용자님 메쉬 축 보정 (X월드-Y메쉬 대응 버전)
			// 포물선을 따라 내려갈 때 부리가 아래를 향하도록 -TargetRot.Pitch 적용
			FRotator VisualFollowRot = FRotator(0.0f, TargetRot.Yaw - 90.0f, -TargetRot.Pitch);

			// 3. 물리 회전과 충돌하지 않도록 강제 세팅 (Interp 속도를 높임)
			FRotator CurrentRot = BirdMesh->GetComponentRotation();
			FRotator NewRot = FMath::RInterpTo(CurrentRot, VisualFollowRot, DeltaTime, 12.0f);

			// TeleportFlag를 써서 물리 연산 중에도 위치/회전을 강제로 맞춥니다.
			BirdMesh->SetWorldRotation(NewRot, false, nullptr, ETeleportType::TeleportPhysics);
		}
	}
}




