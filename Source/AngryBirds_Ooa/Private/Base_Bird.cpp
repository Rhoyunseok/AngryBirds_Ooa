#include "Base_Bird.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "Kismet/GameplayStatics.h"

ABase_Bird::ABase_Bird()
{
    PrimaryActorTick.bCanEverTick = true;

    RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
    RootComponent = RootScene;

    BirdMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BirdMesh"));
    BirdMesh->SetupAttachment(RootScene);
    
    BirdMesh->SetSimulatePhysics(false); 
    BirdMesh->SetCollisionProfileName(TEXT("PhysicsActor"));

    SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    SpringArm->SetupAttachment(RootComponent); 
    SpringArm->TargetArmLength = 800.0f;
    
    SpringArm->SetRelativeRotation(FRotator::ZeroRotator);
    SpringArm->bInheritPitch = true; 
    SpringArm->bInheritYaw = true; 
    SpringArm->bInheritRoll = true;

    SpringArm->bUsePawnControlRotation = false;
    SpringArm->bEnableCameraLag = true;
    SpringArm->CameraLagSpeed = 10.0f;
    
    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);

    bUseCustomPhysics = false;
    GravityConstant = FVector(0.0f, 0.0f, -980.0f); 
}

void ABase_Bird::BeginPlay()
{
    Super::BeginPlay();
}

void ABase_Bird::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
    PlayerInputComponent->BindAction("UseAbility", IE_Pressed, this, &ABase_Bird::OnAbilityInput);
}

void ABase_Bird::OnAbilityInput()
{
    if (bHasLaunched && !bHasHitSomething && !bAbilityUsed)
    {
        UseAbility();
        bAbilityUsed = true;
    }
}

void ABase_Bird::UseAbility()
{
    UE_LOG(LogTemp, Log, TEXT("Base Bird Ability Triggered"));
}

void ABase_Bird::LaunchByVector(FVector LaunchVelocity)
{
    if (!BirdMesh) return;

    // ★ [핵심 추가] 부모(새총)로부터 완전히 분리합니다. 
    // 부착된 상태에서는 위치 변경(SetActorLocation)이 무시됩니다.
    this->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

    // ★ [핵심 추가] 모빌리티를 Movable로 강제 설정
    RootComponent->SetMobility(EComponentMobility::Movable);

    BirdMesh->SetSimulatePhysics(false); 
    BirdMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    
    // 발사 방향으로 즉시 회전
    SetActorRotation(LaunchVelocity.Rotation());
    BirdMesh->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));

    CustomVelocity = LaunchVelocity;
    bUseCustomPhysics = true; 
    bHasLaunched = true;
    LaunchTime = GetWorld()->GetTimeSeconds();

    UE_LOG(LogTemp, Warning, TEXT("커스텀 물리 발사 시작! 속도: %s"), *CustomVelocity.ToString());
}

void ABase_Bird::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bUseCustomPhysics && !bHasHitSomething)
    {
        HandleCustomPhysics(DeltaTime);
    }
}

void ABase_Bird::HandleCustomPhysics(float DeltaTime)
{
    FVector CurrentLocation = GetActorLocation();
    
    // 중력 가속도 적용
    CustomVelocity += GravityConstant * DeltaTime;
    
    // 다음 위치 (P = P0 + V*t)
    FVector NextLocation = CurrentLocation + (CustomVelocity * DeltaTime);

    // 진행 방향으로 회전 동기화
    if (CustomVelocity.Size() > 20.0f)
    {
        SetActorRotation(CustomVelocity.Rotation());
        BirdMesh->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
    }

    // ★ [핵심 수정] TeleportPhysics 옵션을 사용하여 위치를 강제 이동시킵니다.
    FHitResult HitResult;
    bool bMoved = SetActorLocation(NextLocation, true, &HitResult, ETeleportType::TeleportPhysics);

    // 이동 중 장애물 충돌 감지
    if (HitResult.bBlockingHit)
    {
        // 발사 직후 자기 자신/새총 충돌 방지 (0.1초 유예)
        if (GetWorld()->GetTimeSeconds() - LaunchTime > 0.1f)
        {
            bUseCustomPhysics = false;
            OnBirdHit(HitResult.GetComponent(), HitResult.GetActor(), nullptr, FVector::ZeroVector, HitResult);
        }
    }
}

void ABase_Bird::OnBirdHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    if (bHasHitSomething) return;

    bHasHitSomething = true;
    UE_LOG(LogTemp, Warning, TEXT("커스텀 물리 충돌 감지!"));

    GetWorldTimerManager().SetTimer(DespawnTimerHandle, this, &ABase_Bird::StartCameraReturn, 2.0f, false);
}

void ABase_Bird::StartCameraReturn()
{
    APlayerController* PC = GetWorld()->GetFirstPlayerController();
    if (PC && ReturnTarget)
    {
        PC->SetViewTargetWithBlend(ReturnTarget, 1.5f, VTBlend_Cubic);
    }

    FTimerHandle DestroyTimerHandle;
    GetWorldTimerManager().SetTimer(DestroyTimerHandle, this, &ABase_Bird::DestroyBird, 1.6f, false);
}

void ABase_Bird::DestroyBird()
{
    this->Destroy();
}

void ABase_Bird::Launch(FVector LaunchVelocity)
{
    LaunchByVector(LaunchVelocity);
}