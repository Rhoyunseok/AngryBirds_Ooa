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
            // Tick과 동일한 부호 적용 (거울 반사)
            float PitchAngle = FMath::Clamp((DragDelta.Y / MaxDragDist) * 30.0f, -30.0f, 30.0f);
            float YawAngle   = FMath::Clamp((-DragDelta.X / MaxDragDist) * 30.0f, -30.0f, 30.0f);

            FVector LaunchDir = FRotator(PitchAngle, YawAngle, 0.f).Vector();
            float Power = FMath::Clamp(DragDelta.Size() / MaxDragDist, 0.0f, 1.0f) * 6000.0f;

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

    AddActorWorldOffset(FVector(0, 0, 100.0f));
    BirdMesh->SetSimulatePhysics(true);
    BirdMesh->SetAllPhysicsLinearVelocity(FVector::ZeroVector);
    BirdMesh->SetAllPhysicsAngularVelocityInDegrees(FVector::ZeroVector);

    // 공중 회전 잠금
    BirdMesh->BodyInstance.bLockXRotation = true;
    BirdMesh->BodyInstance.bLockYRotation = true;
    BirdMesh->BodyInstance.bLockZRotation = true;
    BirdMesh->BodyInstance.UpdatePhysicsFilterData();

    SetActorRotation(LaunchVelocity.Rotation());
    BirdMesh->SetAllPhysicsLinearVelocity(LaunchVelocity, false);

    if (ProjectileMovement) {
        ProjectileMovement->Velocity = LaunchVelocity;
        ProjectileMovement->Activate(true);
    }
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
            // 마우스 드래그 차이 계산
            FVector2D DragDelta = CurrentMouse - StartMousePos;

            // [의도 반영: 거울 반사 로직]
            // 1. 상하(Pitch): 마우스 위(Y-)로 당기면 새는 아래(Pitch-)를 봐야 함 -> DragDelta.Y 부호 그대로 사용
            // 2. 좌우(Yaw): 마우스 오른쪽(X+)으로 당기면 새는 왼쪽(Yaw-)을 봐야 함 -> -DragDelta.X 사용
            float PitchAngle = FMath::Clamp((DragDelta.Y / MaxDragDist) * 30.0f, -30.0f, 30.0f);
            float YawAngle   = FMath::Clamp((-DragDelta.X / MaxDragDist) * 30.0f, -30.0f, 30.0f);

            // X축(정면) 기준으로 회전값 적용
            FRotator LaunchRot = FRotator(PitchAngle, YawAngle, 0.f);
            SetActorRotation(LaunchRot);

            // [장전 위치 연출]
            // 새가 바라보는 방향(LaunchRot.Vector())의 정반대로 밀기 위해 -1.0 곱함
            float StrengthRatio = FMath::Clamp(DragDelta.Size() / MaxDragDist, 0.0f, 1.0f);
            FVector BackwardOffset = LaunchRot.Vector() * -1.0f * (StrengthRatio * MaxVisualDragDist);
            
            SetActorLocation(InitialLocation + BackwardOffset);

            // 디버그로 수치 확인 (P가 마이너스면 아래, Y가 마이너스면 왼쪽 조준)
            GEngine->AddOnScreenDebugMessage(0, 0.0f, FColor::Cyan, 
                FString::Printf(TEXT("Pitch(상하): %.1f | Yaw(좌우): %.1f"), PitchAngle, YawAngle));
        }
    }
}