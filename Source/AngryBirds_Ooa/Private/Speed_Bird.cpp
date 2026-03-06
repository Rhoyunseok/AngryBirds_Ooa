#include "Speed_Bird.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/PlayerCameraManager.h"

ASpeed_Bird::ASpeed_Bird() : Super()
{
    // 대시 파워는 기존 속도에 더해질 값이므로 조절
    DashPower = 3000.0f;

    // Tick 활성화 (FOV 업데이트를 위해 필요)
    PrimaryActorTick.bCanEverTick = true;

    // 헤더에 정의된 초기값 유지 (필요 시 여기서 수정 가능)
    DashFOV = 110.0f;
    FOVInterpSpeed = 5.0f;
    bIsDashing = false;
}

void ASpeed_Bird::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // 카메라 FOV 조절 (속도감 연출)
    APlayerController* PC = Cast<APlayerController>(GetController());
    if (PC && PC->PlayerCameraManager)
    {
        float CurrentFOV = PC->PlayerCameraManager->GetFOVAngle();
        
        // 대시 중이면 DashFOV(110), 아니면 기본값(90)을 목표로 설정
        // bHasHitSomething은 부모(Base_Bird)에 정의되어 있다고 가정합니다.
        float TargetFOV = (bIsDashing && !bHasHitSomething) ? DashFOV : 90.0f;

        // 현재 FOV에서 목표 FOV로 부드럽게 보간
        if (!FMath::IsNearlyEqual(CurrentFOV, TargetFOV, 0.1f))
        {
            float NewFOV = FMath::FInterpTo(CurrentFOV, TargetFOV, DeltaTime, FOVInterpSpeed);
            PC->PlayerCameraManager->SetFOV(NewFOV);
        }
    }
}

void ASpeed_Bird::UseAbility()
{
    // 1. 이미 능력을 썼거나 부딪혔으면 무시
    if (bAbilityUsed || bHasHitSomething || !bHasLaunched) return;

    // [추가] 부모에 정의된 공통 효과 함수 호출 (사운드 + 이펙트)
    PlayAbilityEffects();

    // [추가] 대시 시 속도감을 위해 경로 연기(Trail) 생성 간격을 0.05초로 단축
    // TrailTimerHandle은 부모의 protected 변수이므로 직접 접근하여 재설정 가능합니다.
    GetWorldTimerManager().SetTimer(TrailTimerHandle, this, &ABase_Bird::SpawnTrail, 0.05f, true);

    // [시각 효과 활성화] Tick에서 FOV를 변경하도록 플래그 전달
    bIsDashing = true;

    // 2. 현재 이동 방향(CustomVelocity)을 기준으로 가속
    if (CustomVelocity.IsNearlyZero())
    {
       CustomVelocity = GetActorForwardVector() * DashPower;
    }
    else
    {
       // 현재 진행 방향으로 속도를 크게 증가시킴
       FVector DashDirection = CustomVelocity.GetSafeNormal();
       CustomVelocity = DashDirection * (CustomVelocity.Size() + DashPower);
    }

    // 3. 시각적 피드백
    SetActorRotation(CustomVelocity.Rotation());

    bAbilityUsed = true;
    UE_LOG(LogTemp, Warning, TEXT("Speed Bird: Dash! New Velocity Size: %f"), CustomVelocity.Size());
}

void ASpeed_Bird::OnBirdHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    // 부모의 OnBirdHit를 호출하여 SimulatePhysics를 켜고 CustomVelocity를 전달함
    Super::OnBirdHit(HitComponent, OtherActor, OtherComp, NormalImpulse, Hit);

    // 충돌했으므로 대시 효과 중지 (Tick에서 FOV가 원래대로 돌아감)
    bIsDashing = false;

    if (BirdMesh)
    {
       // 부딪힌 후 너무 떡처럼 붙지 않게 Damping 값을 적절히 조절
       BirdMesh->SetLinearDamping(0.8f); 
       BirdMesh->SetAngularDamping(0.5f);
    }
}