#include "Base_Bird.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/PlayerController.h"
#include "Framework/Application/NavigationConfig.h"
#include "Engine/EngineTypes.h"

// =============================================================
// 생성자: 컴포넌트 구성 및 기본 초기화
// =============================================================
ABase_Bird::ABase_Bird()
{
    PrimaryActorTick.bCanEverTick = true;

    // 1. Root 설정: 액터의 기준점
    RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
    RootComponent = RootScene;

    // 2. 메쉬 설정: 물리 시뮬레이션 활성화
    BirdMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BirdMesh"));
    BirdMesh->SetupAttachment(RootScene);
    BirdMesh->SetSimulatePhysics(true);
    BirdMesh->SetCollisionProfileName(TEXT("PhysicsActor"));

    // 3. 스프링 암 설정: 카메라가 새를 따라다니되, 새의 회전값은 무시(고정 시야)
    SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    SpringArm->SetupAttachment(BirdMesh); 
    SpringArm->TargetArmLength = 800.0f;
    SpringArm->bInheritPitch = false; // 새의 Pitch 회전 무시
    SpringArm->bInheritYaw = false;   // 새의 Yaw 회전 무시
    SpringArm->bInheritRoll = false;  // 새의 Roll 회전 무시

    // 카메라 래그: 카메라 이동을 부드럽게 지연시킴
    SpringArm->bEnableCameraLag = true;
    SpringArm->CameraLagSpeed = 3.0f;

    // 4. 카메라 설정
    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);

    // 5. 무브먼트 설정: 발사체 로직을 위한 컴포넌트 (주로 속도 제한용)
    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
    ProjectileMovement->UpdatedComponent = BirdMesh;
    ProjectileMovement->bAutoActivate = false;
    ProjectileMovement->MaxSpeed = 10000.f;
}

// =============================================================
// 게임 시작 및 입력 바인딩
// =============================================================
void ABase_Bird::BeginPlay()
{
    Super::BeginPlay();

    APlayerController* PC = Cast<APlayerController>(GetWorld()->GetFirstPlayerController());
    if (PC)
    {
       EnableInput(PC);
       PC->bShowMouseCursor = true;
       FInputModeGameAndUI InputMode;
       PC->SetInputMode(InputMode);
    }

    if (BirdMesh)
    {
       // 물리 충돌 이벤트 연결 및 알림 활성화
       BirdMesh->OnComponentHit.AddDynamic(this, &ABase_Bird::OnBirdHit);
       BirdMesh->SetNotifyRigidBodyCollision(true);
    }
}

void ABase_Bird::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
    // 드래그 액션 바인딩 (누를 때 시작, 뗄 때 발사)
    PlayerInputComponent->BindAction("DragShoot", IE_Pressed, this, &ABase_Bird::OnDragStart);
    PlayerInputComponent->BindAction("DragShoot", IE_Released, this, &ABase_Bird::OnDragRelease);
}

// =============================================================
// 드래그 및 발사 로직
// =============================================================
void ABase_Bird::OnDragStart()
{
    APlayerController* PC = Cast<APlayerController>(GetWorld()->GetFirstPlayerController());
    if (PC && PC->GetMousePosition(StartMousePos.X, StartMousePos.Y))
    {
       if (StartMousePos.IsZero()) return;

       bIsDragging = true;
       InitialLocation = GetActorLocation(); // 조준 시작 위치 저장

       // 드래그 중 물리 일시 정지 및 충돌 비활성화 (마우스 조작 우선)
       BirdMesh->SetSimulatePhysics(false);
       BirdMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

       FInputModeGameAndUI InputMode;
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

       // 최소 드래그 거리(20) 체크
       if (DragDelta.Size() > 20.0f)
       {
          // 1. Yaw(좌우): X축 드래그 비례 (-45~45도)
          float YawAngle = FMath::Clamp((-DragDelta.X / MaxDragDist) * 45.0f, -45.0f, 45.0f);

          // 2. Pitch(상하): 상하/좌우 드래그 차이를 이용한 고도 계산 (0~90도)
          float VerticalBias = (FMath::Abs(DragDelta.Y) - FMath::Abs(DragDelta.X)) / MaxDragDist;
          float PitchAngle = FMath::Clamp(VerticalBias * 90.0f, 0.0f, 90.0f);

          // 3. 발사 방향 벡터 생성
          FVector LaunchDir = FRotator(PitchAngle, YawAngle, 0.f).Vector();

          // 4. 발사 파워 계산: 드래그 길이에 비례 (최대 2500)
          float Power = FMath::Clamp(DragDelta.Size() / MaxDragDist, 0.0f, 1.0f) * 2500.0f;

          LaunchByVector(LaunchDir * Power);
       }
       else
       {
          // 드래그가 짧으면 원래 위치로 복구 및 물리 재활성화
          SetActorLocation(InitialLocation);
          BirdMesh->SetSimulatePhysics(true);
       }
    }
}

void ABase_Bird::LaunchByVector(FVector LaunchVelocity)
{
    if (!BirdMesh) return;

    // 발사 방향으로 즉시 회전 후 물리 및 충돌 복구
    SetActorRotation(LaunchVelocity.Rotation());
    BirdMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    BirdMesh->SetSimulatePhysics(true);

    // 물리 회전 잠금 해제: 비행 중 자유로운 회전 허용
    BirdMesh->BodyInstance.bLockXRotation = false;
    BirdMesh->BodyInstance.bLockYRotation = false;
    BirdMesh->BodyInstance.bLockZRotation = false;
    BirdMesh->BodyInstance.UpdatePhysicsFilterData();

    // 초기 속도 적용 및 발사 상태 플래그 활성화
    BirdMesh->SetAllPhysicsLinearVelocity(LaunchVelocity, false);
    bHasLaunched = true;
}

void ABase_Bird::Launch(float VerticalAngle, float HorizontalAngle, float Power)
{
    // 수동 발사를 위한 보조 함수 (각도 기반)
    float VRad = FMath::DegreesToRadians(VerticalAngle);
    float HRad = FMath::DegreesToRadians(HorizontalAngle);
    FVector Dir(FMath::Cos(VRad) * FMath::Sin(HRad), FMath::Cos(VRad) * FMath::Cos(HRad), FMath::Sin(VRad));
    LaunchByVector(Dir.GetSafeNormal() * Power);
}

// =============================================================
// 틱 업데이트: 조준 비주얼 및 비행 중 회전 제어
// =============================================================
void ABase_Bird::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // [상태 1] 드래그 조준 중
    if (bIsDragging)
    {
       APlayerController* PC = Cast<APlayerController>(GetWorld()->GetFirstPlayerController());
       FVector2D CurrentMouse;
       if (PC && PC->GetMousePosition(CurrentMouse.X, CurrentMouse.Y))
       {
          FVector2D DragDelta = CurrentMouse - StartMousePos;

          // 1. 조준 각도 계산
          float PitchAngle = FMath::Clamp((DragDelta.Y / MaxDragDist) * 30.0f, -30.0f, 30.0f);
          float YawAngle = FMath::Clamp((-DragDelta.X / MaxDragDist) * 30.0f, -30.0f, 30.0f);
          float HeightBias = (FMath::Abs(DragDelta.Y) - FMath::Abs(DragDelta.X)) / MaxDragDist;
          float FinalPitch = FMath::Clamp(PitchAngle + (HeightBias * 45.0f), -20.0f, 90.0f);

          // 2. 비주얼 회전 보정 (메쉬 축 방향에 따른 수치 조정)
          FRotator VisualRot = FRotator(0.0f, YawAngle - 90.0f, -FinalPitch);
          if (BirdMesh) BirdMesh->SetWorldRotation(VisualRot);

          // 3. 시각적 위치 이동: 발사 방향 반대쪽으로 당겨지는 연출
          float StrengthRatio = FMath::Clamp(DragDelta.Size() / MaxDragDist, 0.0f, 1.0f);
          FVector LaunchDir = FRotator(FinalPitch, YawAngle, 0.0f).Vector();
          FVector BackwardOffset = LaunchDir * -1.0f * (StrengthRatio * MaxVisualDragDist);
          SetActorLocation(InitialLocation + BackwardOffset);
          
          // 4. 예상 궤적 표시
          DisplayTrajectory();

          // 디버그 메시지 출력
          GEngine->AddOnScreenDebugMessage(0, 0.0f, FColor::Cyan,
             FString::Printf(TEXT("Pitch(상하): %.1f | Yaw(좌우): %.1f"), PitchAngle, YawAngle));
       }
    }
    // [상태 2] 발사 후 비행 중
    else if (bHasLaunched && !bHasHitSomething)
    {
       FVector CurrentVelocity = BirdMesh->GetPhysicsLinearVelocity();
       if (CurrentVelocity.Size() > 100.0f)
       {
          // 비행 속도 방향에 맞춰 메쉬의 고개가 향하도록 업데이트 (포물선 연출)
          FRotator TargetRot = CurrentVelocity.Rotation();
          FRotator VisualFollowRot = FRotator(0.0f, TargetRot.Yaw - 90.0f, -TargetRot.Pitch);
          BirdMesh->SetWorldRotation(VisualFollowRot, false, nullptr, ETeleportType::TeleportPhysics);
       }
    }
    // [상태 3] 충돌 후 정지 과정 
    else if (bIsSettling)
    {
       /* 로직 생략 */
    }
}

// =============================================================
// 충돌 처리 및 소멸
// =============================================================
void ABase_Bird::OnBirdHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                           FVector NormalImpulse, const FHitResult& Hit)
{
    if (bHasHitSomething) return;

    if (bHasLaunched)
    {
       bHasHitSomething = true;
       bIsSettling = true;

       // 무브먼트 컴포넌트 즉시 중지
       if (ProjectileMovement)
       {
          ProjectileMovement->StopMovementImmediately();
          ProjectileMovement->Deactivate();
       }

       // 1. 물리 속도 초기화 (반발력 제거)
       BirdMesh->SetPhysicsLinearVelocity(FVector::ZeroVector);
       BirdMesh->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);

       // 2. 무작위 회전 임펄스 (바닥에서 구르는 연출)
       FVector WildSpin = FVector(FMath::RandRange(-10000, 10000), FMath::RandRange(-10000, 10000),
                                  FMath::RandRange(-10000, 10000));
       BirdMesh->AddAngularImpulseInDegrees(WildSpin, NAME_None, true);

       // 3. 댐핑(마찰 저항)을 높여 빠르게 정지하도록 설정
       BirdMesh->SetLinearDamping(20.0f); 
       BirdMesh->SetAngularDamping(0.5f);

       // 4. 3초 후 소멸 타이머 시작
       GetWorldTimerManager().SetTimer(DespawnTimerHandle, this, &ABase_Bird::DestroyBird, 3.0f, false);
    }
}

void ABase_Bird::DestroyBird()
{
    Destroy();
}

// =============================================================
// 궤적 예측 시각화
// =============================================================
void ABase_Bird::DisplayTrajectory()
{
    if (!bIsDragging) return;

    APlayerController* PC = Cast<APlayerController>(GetWorld()->GetFirstPlayerController());
    FVector2D CurrentMouse;
    if (PC && PC->GetMousePosition(CurrentMouse.X, CurrentMouse.Y))
    {
       FVector2D DragDelta = CurrentMouse - StartMousePos;
       
       // 실제 발사 시와 동일한 각도 및 파워 계산
       float PitchAngle = FMath::Clamp(((FMath::Abs(DragDelta.Y) - FMath::Abs(DragDelta.X)) / MaxDragDist) * 90.0f, 0.0f, 90.0f);
       float YawAngle = FMath::Clamp((-DragDelta.X / MaxDragDist) * 45.0f, -45.0f, 45.0f);

       FVector LaunchDir = FRotator(PitchAngle, YawAngle, 0.f).Vector();
       float Power = FMath::Clamp(DragDelta.Size() / MaxDragDist, 0.0f, 1.0f) * 2500.0f;
       FVector LaunchVelocity = LaunchDir * Power;

       // 궤적 시작 위치 보정 (새의 전방 200유닛 지점)
       float ForwardOffset = 200.0f;
       FVector StartPos = GetActorLocation() + (LaunchDir * ForwardOffset);

       // PredictProjectilePath: 물리 기반 궤적을 예측하여 디버그 라인으로 출력
       FPredictProjectilePathParams PathParams(10.0f, StartPos, LaunchVelocity, 2.0f);
       PathParams.bTraceWithChannel = true;
       PathParams.TraceChannel = ECollisionChannel::ECC_WorldStatic;
       PathParams.DrawDebugType = EDrawDebugTrace::ForOneFrame; // 매 프레임 갱신
       PathParams.DrawDebugTime = 0.0f;

       FPredictProjectilePathResult PathResult;
       UGameplayStatics::PredictProjectilePath(this, PathParams, PathResult);
    }
}