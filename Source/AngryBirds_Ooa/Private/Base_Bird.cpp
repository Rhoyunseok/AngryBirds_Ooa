#include "Base_Bird.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "Kismet/GameplayStatics.h" // 카메라 제어를 위해 추가

ABase_Bird::ABase_Bird()
{
    PrimaryActorTick.bCanEverTick = true;

    RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
    RootComponent = RootScene;

    BirdMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BirdMesh"));
    BirdMesh->SetupAttachment(RootScene);
    BirdMesh->SetSimulatePhysics(false); // 새총 발사 전까지 물리 끔
    BirdMesh->SetCollisionProfileName(TEXT("PhysicsActor"));

    SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    SpringArm->SetupAttachment(BirdMesh);
    SpringArm->TargetArmLength = 800.0f;
    // 새의 진행 방향에 평행한 상태로 날아가기 위해서 
    SpringArm->bInheritPitch = false; 
    SpringArm->bInheritYaw = false;
    SpringArm->bInheritRoll = false;

    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);

    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
    ProjectileMovement->UpdatedComponent = BirdMesh;
    ProjectileMovement->bAutoActivate = false;
}

void ABase_Bird::BeginPlay()
{
    Super::BeginPlay();
    if (BirdMesh)
    {
        BirdMesh->OnComponentHit.AddDynamic(this, &ABase_Bird::OnBirdHit);
        BirdMesh->SetNotifyRigidBodyCollision(true);
    }
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

    BirdMesh->SetSimulatePhysics(true); 
    BirdMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    
    // ★ 추가: 발사 직전, 새의 몸통이 날아갈 방향을 정면으로 바라보게 합니다.
    // 모델 에셋이 -90도 돌아가 있다면 여기서도 보정이 필요합니다.
    FRotator LaunchRot = LaunchVelocity.Rotation();
    LaunchRot.Yaw -= 90.0f; // 에셋 보정값 적용
    SetActorRotation(LaunchRot);
    
    // ★ [추가] 물리 엔진이 충격에 의해 새를 핑글핑글 돌리는 것을 방지합니다.
    // 회전은 오직 우리가 Tick에서 계산하는 대로만 움직이게 됩니다.
    BirdMesh->GetBodyInstance()->bLockXRotation = true;
    BirdMesh->GetBodyInstance()->bLockYRotation = true;
    BirdMesh->GetBodyInstance()->bLockZRotation = true;

    BirdMesh->SetAllPhysicsLinearVelocity(LaunchVelocity, false);
    
    // 발사 시간 기록
    LaunchTime = GetWorld()->GetTimeSeconds();
    bHasLaunched = true;
}

void ABase_Bird::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bHasLaunched && !bHasHitSomething)
    {
        FVector CurrentVelocity = BirdMesh->GetPhysicsLinearVelocity();
        if (CurrentVelocity.Size() > 50.0f)
        {
            // 목표 회전값 계산
            FRotator TargetRot = CurrentVelocity.Rotation();
            // 에셋 방향 보정 (사용자 모델에 맞게 조정: 현재 -90 유지)
            FRotator VisualRot = FRotator(TargetRot.Pitch, TargetRot.Yaw - 90.0f, 0.0f);

            // ★ [추가] 현재 회전에서 목표 회전까지 부드럽게 회전하도록 보간(RInterpTo)을 사용합니다.
            FRotator CurrentRot = BirdMesh->GetComponentRotation();
            FRotator SmoothedRot = FMath::RInterpTo(CurrentRot, VisualRot, DeltaTime, 50.0f);
            
            // bTeleport를 true로 설정하여 물리 엔진과의 충돌을 방지합니다.
            BirdMesh->SetWorldRotation(SmoothedRot, false, nullptr, ETeleportType::TeleportPhysics);
            
<<<<<<< HEAD
            BirdMesh->SetWorldRotation(SmoothedRot, false, nullptr, ETeleportType::TeleportPhysics);
=======
>>>>>>> a3bda526f77de150aab24b80f5dcc52e635d2f70
        }
    }
}

void ABase_Bird::OnBirdHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    if (bHasHitSomething) return;

    // 새총과는 이미 IgnoreActor 설정을 했으므로, 여기서 체크하는 시간은 훨씬 짧아도 됩니다.
    if (GetWorld()->GetTimeSeconds() - LaunchTime < 0.1f) return;
    
    bHasHitSomething = true;
    UE_LOG(LogTemp, Warning, TEXT("충돌 발생! 2초 후 시점 복구 예약"));

    // 2초 뒤에 카메라 복귀 시작
    GetWorldTimerManager().SetTimer(DespawnTimerHandle, this, &ABase_Bird::StartCameraReturn, 2.0f, false);
}

void ABase_Bird::StartCameraReturn()
{
    APlayerController* PC = GetWorld()->GetFirstPlayerController();
    if (PC && ReturnTarget)
    {
        PC->SetViewTargetWithBlend(ReturnTarget, 1.5f, VTBlend_Cubic);
    }

    // ★ [수정] 파괴용 핸들을 별도로 사용하여 이전 타이머 핸들(DespawnTimerHandle)과 겹치지 않게 합니다.
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


// 내가 원하는건
/*
 physics simulate 함수를 너가 만드는거야
 그래서 이걸 껐다 켰다 하는거지 engine 있는게 아니라 
 */