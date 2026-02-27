#include "Base_Bird.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h" // 스피어 컴포넌트 추가

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
    
    // ★ [수정] 진행 방향을 바라보도록 설정
    SpringArm->bUsePawnControlRotation = false; // 컨트롤러 회전 대신 액터 회전 사용
    
    // ★ [중요] 일단 모두 true로 둡니다 (그래야 새가 꺾이는 방향을 따라감)
    SpringArm->bInheritPitch = true;
    SpringArm->bInheritYaw = true;
    SpringArm->bInheritRoll = true;

    SpringArm->bEnableCameraLag = true;
    SpringArm->CameraLagSpeed = 5.0f;
    
    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);

    bUseCustomPhysics = false;
    GravityConstant = FVector(0.0f, 0.0f, -980.0f); 
    
    // 충돌 전용 스피어 생성
    CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
    CollisionSphere->SetupAttachment(RootScene);
    
    // 새의 외형보다 확실히 작은 크기 (간섭 방지)
    CollisionSphere->SetSphereRadius(15.0f); 

    CollisionSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    CollisionSphere->SetCollisionResponseToAllChannels(ECR_Block);
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

    this->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
    RootComponent->SetMobility(EComponentMobility::Movable);

    BirdMesh->SetSimulatePhysics(false); 
    BirdMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics); // 물리 충돌을 위해 수정
    
    SetActorRotation(LaunchVelocity.Rotation());
    BirdMesh->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));

    CustomVelocity = LaunchVelocity;
    bUseCustomPhysics = true; 
    bHasLaunched = true;
    LaunchTime = GetWorld()->GetTimeSeconds();

    UE_LOG(LogTemp, Warning, TEXT("커스텀 물리 발사 시작!"));
}

void ABase_Bird::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bUseCustomPhysics && !bHasHitSomething)
    {
        HandleCustomPhysics(DeltaTime);
    }
    // ★ [추가] 카메라 수평 유지 로직
    if (bHasLaunched && SpringArm)
    {
        // 현재 액터(새)의 회전값을 가져옵니다.
        FRotator CurrentRot = GetActorRotation();
        
        // Yaw(좌우 회전)는 새를 따라가되, 
        // Pitch(위아래)와 Roll(기울기)은 고정하여 화면이 뒤집히지 않게 합니다.
        // -20.0f 정도를 주면 약간 위에서 아래로 내려다보는 좋은 각도가 됩니다.
        FRotator DesiredRot = FRotator(-20.0f, CurrentRot.Yaw, 0.0f);
        
        // SpringArm에 직접 월드 회전값을 꽂아넣어 상속을 무시하게 만듭니다.
        SpringArm->SetWorldRotation(DesiredRot);
    }
}

void ABase_Bird::HandleCustomPhysics(float DeltaTime)
{
    FVector CurrentLocation = GetActorLocation();
    CustomVelocity += GravityConstant * DeltaTime;
    FVector NextLocation = CurrentLocation + (CustomVelocity * DeltaTime);

    if (CustomVelocity.Size() > 20.0f)
    {
        SetActorRotation(CustomVelocity.Rotation());
        BirdMesh->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
    }

    float CheckRadius = CollisionSphere ? CollisionSphere->GetScaledSphereRadius() : 15.0f;
    
    FHitResult HitResult;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);

    bool bHit = GetWorld()->SweepSingleByChannel(
        HitResult,
        CurrentLocation,
        NextLocation,
        FQuat::Identity,
        ECC_WorldStatic, 
        FCollisionShape::MakeSphere(CheckRadius),
        Params
    );

    if (bHit)
    {
        // 충돌 지점으로 이동 (약간의 오프셋으로 파고들기 방지)
        SetActorLocation(HitResult.Location + (HitResult.Normal * 2.0f), false, nullptr, ETeleportType::TeleportPhysics);
        
        if (GetWorld()->GetTimeSeconds() - LaunchTime > 0.1f)
        {
            // ★ [핵심 추가] 부딪힌 상대방이 물리 시뮬레이션 중이라면 힘을 가합니다.
            if (HitResult.GetComponent() && HitResult.GetComponent()->IsSimulatingPhysics())
            {
                // 부딪힌 지점에 발사 속도 방향으로 충격(Impulse)을 줍니다.
                // 100.0f는 무게(Mass)에 비례한 힘의 세기입니다. 너무 약하면 수치를 올리세요.
                HitResult.GetComponent()->AddImpulseAtLocation(CustomVelocity * 100.0f, HitResult.ImpactPoint);
            }
            
            // ★ [수정] 부딪혔을 때 속도를 반사시킵니다. (0.5f는 탄성률: 절반의 힘으로 튕김)
            CustomVelocity = FMath::GetReflectionVector(CustomVelocity, HitResult.Normal) * 0.5f;
            
            // OnBirdHit 호출 (이제 이 안에서 물리 시뮬레이션이 켜집니다)
            OnBirdHit(HitResult.GetComponent(), HitResult.GetActor(), nullptr, CustomVelocity, HitResult);
        }
    }
    else
    {
        SetActorLocation(NextLocation, false, nullptr, ETeleportType::TeleportPhysics);
    }
}

void ABase_Bird::OnBirdHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    if (bHasHitSomething) return;
    bHasHitSomething = true;

    // ★ [핵심] 커스텀 물리 계산을 중단하고 엔진 물리를 활성화
    bUseCustomPhysics = false;

    if (BirdMesh)
    {
        BirdMesh->SetSimulatePhysics(true); // 이제부터 엔진이 중력과 구름을 처리
        
        // 튕겨나가는 속도 적용
        BirdMesh->SetPhysicsLinearVelocity(CustomVelocity);
        
        // 찰지게 구르도록 랜덤한 회전력(Spin) 추가
        FVector RandomSpin = FVector(FMath::FRandRange(-1.f, 1.f), FMath::FRandRange(-1.f, 1.f), FMath::FRandRange(-1.f, 1.f)) * 500.0f;
        BirdMesh->SetPhysicsAngularVelocityInDegrees(RandomSpin);
    }

    UE_LOG(LogTemp, Warning, TEXT("충돌 발생! 데굴데굴 모드 전환"));

    // 구르는 걸 더 잘 볼 수 있게 카메라 복귀 대기 시간을 3.5초로 늘림
    GetWorldTimerManager().SetTimer(DespawnTimerHandle, this, &ABase_Bird::StartCameraReturn, 3.5f, false);
}

void ABase_Bird::StartCameraReturn()
{
    APlayerController* PC = GetWorld()->GetFirstPlayerController();
    if (PC)
    {
        // 1. 월드에서 "MainCamera" 태그를 가진 모든 액터를 찾습니다.
        TArray<AActor*> FoundActors;
        UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("MainCamera"), FoundActors);
        
        if (FoundActors.Num() > 0)
        {
            // 2. 첫 번째로 찾은 카메라 액터로 화면을 부드럽게 돌립니다.
            PC->SetViewTargetWithBlend(FoundActors[0], 1.5f, VTBlend_Cubic);
            UE_LOG(LogTemp, Warning, TEXT("태그로 메인 카메라를 찾았습니다!"));
        }
        else if (ReturnTarget)
        {
            // 태그로 못 찾았을 경우를 대비한 예외 처리
            PC->SetViewTargetWithBlend(ReturnTarget, 1.5f, VTBlend_Cubic);
        }
    }

    // 카메라 전환 중에 새가 사라지면 어색하므로 타이머를 맞춥니다.
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