#include "Base_Bird.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/PlayerController.h"

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
    if (PC && PC->GetMousePosition(StartMousePos.X, StartMousePos.Y))
    {
        bIsDragging = true;
        InitialLocation = GetActorLocation();
        BirdMesh->SetSimulatePhysics(false);
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
        if (DragDelta.Size() < 20.0f) {
            SetActorLocation(InitialLocation);
            BirdMesh->SetSimulatePhysics(true);
            return;
        }

        FVector LaunchDir;
        LaunchDir.Y = FMath::Max(0.0f, DragDelta.Y); 
        LaunchDir.X = DragDelta.X;        
        LaunchDir.Z = DragDelta.Size() * 0.4f; 

        float PowerRatio = FMath::Clamp(DragDelta.Size() / MaxDragDist, 0.0f, 1.0f);
        LaunchByVector(LaunchDir.GetSafeNormal() * (PowerRatio * 6000.0f));
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
            FVector2D Delta = CurrentMouse - StartMousePos;
            FVector NewLoc = InitialLocation;
            float RatioX = FMath::Clamp(Delta.X / MaxDragDist, -1.0f, 1.0f);
            float RatioY = FMath::Clamp(Delta.Y / MaxDragDist, -1.0f, 1.0f);
            NewLoc.X -= RatioX * MaxVisualDragDist;
            NewLoc.Y -= RatioY * MaxVisualDragDist;
            SetActorLocation(NewLoc);
        }
    }
}