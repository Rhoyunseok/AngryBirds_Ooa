#include "Base_Bird.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "Kismet/GameplayStatics.h"
#include "SlingShot.h"
#include "Components/SphereComponent.h" // 스피어 컴포넌트 추가
#include "NiagaraFunctionLibrary.h"   // 나이아가라 라이브러리 추가
#include "NiagaraSystem.h"
#include "RHO/AngryBirdGameState.h"

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
    
    // 진행 방향을 바라보도록 설정
    SpringArm->bUsePawnControlRotation = false; // 컨트롤러 회전 대신 액터 회전 사용
    
    // 일단 모두 true로 두어 새가 꺾이는 방향을 따라가게 설정
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
    
    // 기본 무게 초기화
    BirdWeight = 10.0f;
    
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

void ABase_Bird::PlayBirdSound(USoundBase* SoundToPlay)
{
    if (SoundToPlay)
    {
        UGameplayStatics::PlaySoundAtLocation(this, SoundToPlay, GetActorLocation());
    }
}

void ABase_Bird::PlayReadyVoice()
{
    PlayBirdSound(ReadyVoiceSound); // "으랴앗!" 기합
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

    // [추가] 경로 연기 타이머 시작 (0.1초마다 SpawnTrail 호출하여 봉봉봉봉 효과)
    GetWorldTimerManager().SetTimer(TrailTimerHandle, this, &ABase_Bird::SpawnTrail, 0.15f, true);

    // 발사 보이스 재생 ("포잉~~~")
    PlayBirdSound(FlyingVoiceSound);

    UE_LOG(LogTemp, Warning, TEXT("커스텀 물리 발사 시작! 포잉~~~"));
}

void ABase_Bird::SpawnTrail()
{
    if (bHasLaunched && !bHasHitSomething && FlyingTrailEffect)
    {
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(
            GetWorld(), 
            FlyingTrailEffect, 
            GetActorLocation(), 
            GetActorRotation()
        );
    }
    else
    {
        // 더 이상 필요 없으면 타이머 종료
        GetWorldTimerManager().ClearTimer(TrailTimerHandle);
    }
}

void ABase_Bird::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bUseCustomPhysics && !bHasHitSomething)
    {
        HandleCustomPhysics(DeltaTime);
    }

    // 카메라 수평 유지 로직
    if (bHasLaunched && SpringArm)
    {
        // 현재 액터(새)의 회전값을 가져옵니다.
        FRotator CurrentRot = GetActorRotation();
        
        // Yaw(좌우 회전)는 새를 따라가되, 
        // Pitch(위아래)와 Roll(기울기)은 고정하여 화면이 뒤집히지 않게 합니다.
        FRotator DesiredRot = FRotator(-20.0f, CurrentRot.Yaw, 0.0f);
        
        // SpringArm에 직접 월드 회전값을 꽂아넣어 상속을 무시하게 만듭니다.
        SpringArm->SetWorldRotation(DesiredRot);
    }
}

void ABase_Bird::HandleCustomPhysics(float DeltaTime)
{
    FVector CurrentLocation = GetActorLocation();
    // 무게에 따른 중력 가속도 계산
    // 기준 무게를 10.0으로 잡고, 무거울수록(20.0) 더 빨리 떨어지게 설정합니다.
    // 무게가 20이면 중력을 2배(2.0)로 받고, 8이면 0.8배로 받습니다.
    float GravityScale = BirdWeight / 10.0f;
    
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
            // 충돌 신음 사운드 ("퍽!") 및 이펙트 재생
            PlayBirdSound(PainVoiceSound);
            if (HitParticle)
            {
                UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitParticle, HitResult.ImpactPoint);
            }

            // 부딪힌 상대방이 물리 시뮬레이션 중이라면 힘을 가합니다.
            if (HitResult.GetComponent() && HitResult.GetComponent()->IsSimulatingPhysics())
            {
                // 부딪힌 지점에 발사 속도 방향으로 충격(Impulse)을 줍니다.
                HitResult.GetComponent()->AddImpulseAtLocation(CustomVelocity * 100.0f, HitResult.ImpactPoint);
            }
            
            // 부딪혔을 때 속도를 반사시킵니다. (0.5f는 탄성률: 절반의 힘으로 튕김)
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

    // 1. 트레일 중단 및 물리 설정
    GetWorldTimerManager().ClearTimer(TrailTimerHandle);
    bUseCustomPhysics = false;

    if (BirdMesh)
    {
        // [추가] 물리 엔진의 질량(Mass)을 우리가 설정한 변수값으로 강제 적용
        BirdMesh->SetMassOverrideInKg(NAME_None, BirdWeight, true);
        
        BirdMesh->SetSimulatePhysics(true);
        BirdMesh->SetPhysicsLinearVelocity(CustomVelocity);
        FVector RandomSpin = FVector(FMath::FRandRange(-1.f, 1.f), FMath::FRandRange(-1.f, 1.f), FMath::FRandRange(-1.f, 1.f)) * 500.0f;
        BirdMesh->SetPhysicsAngularVelocityInDegrees(RandomSpin);
    }

    UE_LOG(LogTemp, Warning, TEXT("충돌 발생! 서브 카메라로 전환합니다."));

    // 2. [추가] SubCamera 태그를 가진 액터로 먼저 이동
    APlayerController* PC = GetWorld()->GetFirstPlayerController();
    if (PC)
    {
        TArray<AActor*> SubActors;
        UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("SubCamera"), SubActors);
        
        if (SubActors.Num() > 0)
        {
            // 서브 카메라로 0.5초 동안 부드럽게 블렌딩 (즉시 전환을 원하면 0.0f)
            PC->SetViewTargetWithBlend(SubActors[0], 0.5f, VTBlend_Cubic);
        }
    }

    // 3. 4초 뒤에 메인 카메라로 돌아가는 함수 호출
    GetWorldTimerManager().SetTimer(DespawnTimerHandle, this, &ABase_Bird::StartCameraReturn, 4.0f, false);
}

void ABase_Bird::StartCameraReturn()
{
    APlayerController* PC = GetWorld()->GetFirstPlayerController();
    if (PC)
    {
        // 월드에서 "MainCamera" 태그를 가진 액터를 찾음
        TArray<AActor*> MainActors;
        UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("MainCamera"), MainActors);
        
        if (MainActors.Num() > 0)
        {
            // 메인 카메라로 1.5초 동안 부드럽게 복귀
            PC->SetViewTargetWithBlend(MainActors[0], 1.5f, VTBlend_Cubic);
            UE_LOG(LogTemp, Warning, TEXT("메인 카메라로 복귀 중..."));
        }
    }

    // 카메라가 메인으로 돌아가는 시간(1.5초)을 고려하여 새 파괴 타이머 설정
    FTimerHandle DestroyTimerHandle;
    GetWorldTimerManager().SetTimer(DestroyTimerHandle, this, &ABase_Bird::DestroyBird, 3.6f, false);
}

void ABase_Bird::DestroyBird()
{
    // 새가 파괴되기 직전에 새총에게 새로운 새를 장전하라고 명령합니다.
    if (ReturnTarget)
    {
        // ReturnTarget이 ASlingShot 클래스인지 확인하고 형변환합니다.
        ASlingShot* Slingshot = Cast<ASlingShot>(ReturnTarget);
        if (Slingshot)
        {
            Slingshot->LoadBird(); // 새로운 새 스폰 및 파우치에 부착
        }
    }
    // 딜레이 주기
    
    
    this->Destroy();
    AAngryBirdGameState* GameState = Cast<AAngryBirdGameState>(UGameplayStatics::GetGameState(this));
    
    if (GameState)
    {
        GameState->ActiveBirdsOnField--;
        // 2. 심판에게 "나 죽었으니, 혹시 이게 마지막 새였고 돼지도 남았으면 패배 처리해!" 라고 알림
        GameState->CheckMatchState();
    }
}

void ABase_Bird::Launch(FVector LaunchVelocity)
{
    LaunchByVector(LaunchVelocity);
}

void ABase_Bird::PlayAbilityEffects()
{
    // 1. 사운드 재생
    if (AbilityVoiceSound) PlayBirdSound(AbilityVoiceSound);

    // 2. 이펙트 재생 (캐스케이드 우선 순위)
    if (HitParticle) // 붐버드에서 P_Explosion_Smoke를 넣었을 때
    {
        UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitParticle, GetActorLocation(), GetActorRotation(), FVector(3.0f));
    }
    else if (AbilityNiagaraEffect) // 스피드버드에서 대시 이펙트를 넣었을 때
    {
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), AbilityNiagaraEffect, GetActorLocation(), GetActorRotation());
    }
}