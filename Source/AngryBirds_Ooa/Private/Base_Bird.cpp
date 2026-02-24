#include "Base_Bird.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"

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

// [수정된 부분] 입력 바인딩
void ABase_Bird::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    // &ABase_Bird::OnAbilityInput <- 이 부분이 헤더의 이름과 일치하는지 꼭 확인하세요!
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
    // 기본 구현 (로그만 출력)
    UE_LOG(LogTemp, Log, TEXT("Base Bird Ability Triggered"));
}

void ABase_Bird::LaunchByVector(FVector LaunchVelocity)
{
    if (!BirdMesh) return;

    BirdMesh->SetSimulatePhysics(true);
    BirdMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    
    // 속도 적용
    BirdMesh->SetAllPhysicsLinearVelocity(LaunchVelocity, false);
    bHasLaunched = true;
}

void ABase_Bird::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // 날아가는 동안 앞을 바라보게 함
    if (bHasLaunched && !bHasHitSomething)
    {
        FVector CurrentVelocity = BirdMesh->GetPhysicsLinearVelocity();
        if (CurrentVelocity.Size() > 100.0f)
        {
            FRotator TargetRot = CurrentVelocity.Rotation();
            // 메쉬의 기본 방향에 따라 보정 (필요시 조정)
            FRotator VisualRot = FRotator(0.0f, TargetRot.Yaw - 90.0f, -TargetRot.Pitch);
            BirdMesh->SetWorldRotation(VisualRot, false, nullptr, ETeleportType::TeleportPhysics);
        }
    }
}

void ABase_Bird::OnBirdHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    if (bHasHitSomething) return;
    bHasHitSomething = true;

    // 3초 뒤에 삭제
    GetWorldTimerManager().SetTimer(DespawnTimerHandle, this, &ABase_Bird::DestroyBird, 3.0f, false);
}

void ABase_Bird::DestroyBird()
{
    Destroy();
}

void ABase_Bird::Launch(FVector LaunchVelocity)
{
    LaunchByVector(LaunchVelocity);
}