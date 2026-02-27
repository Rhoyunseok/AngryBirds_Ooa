#include "Bomb_Bird.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "DrawDebugHelpers.h"

ABomb_Bird::ABomb_Bird()
{
    ExplosionRadius = 900.0f;
    ExplosionStrength = 7000.0f;
}

void ABomb_Bird::UseAbility()
{
    // 이미 능력을 사용했거나 아직 발사 전이면 무시
    // bAbilityUsed, bHasLaunched는 부모인 Base_Bird의 멤버 변수입니다.
    if (bAbilityUsed || !bHasLaunched) return;

    // 비행 중 클릭 시 즉시 폭발
    if (!bHasHitSomething)
    {
        UE_LOG(LogTemp, Warning, TEXT("BombBird: 공중 폭발 실행!"));
        Explode();
    }
}

void ABomb_Bird::OnBirdHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    // 부모의 OnBirdHit를 먼저 호출하여 기본적인 상태 변경(bHasHitSomething = true 등)을 수행
    Super::OnBirdHit(HitComponent, OtherActor, OtherComp, NormalImpulse, Hit);

    // 충돌 시 능력을 아직 사용하지 않았다면 1초 뒤 자동 폭발 예약
    if (!bAbilityUsed)
    {
        bAbilityUsed = true; // 중복 예약 방지
        UE_LOG(LogTemp, Log, TEXT("BombBird: 충돌 확인, 1초 후 지연 폭발"));

        GetWorldTimerManager().SetTimer(BombTimerHandle, this, &ABomb_Bird::Explode, 1.0f, false);
    }
}

void ABomb_Bird::Explode()
{
    // 중복 실행 방지
    if (!BirdMesh || !bHasLaunched) return;
    
    // 이미 폭발 프로세스가 시작되었다면 무시 (TimerHandle 등으로 체크 가능)
    GetWorldTimerManager().ClearTimer(BombTimerHandle);
    bAbilityUsed = true;

    // 1. 물리 및 이동 중단 (카메라 고정 효과)
    bUseCustomPhysics = false; 
    CustomVelocity = FVector::ZeroVector; 
    
    if (BirdMesh)
    {
        BirdMesh->SetSimulatePhysics(false);
        BirdMesh->SetAllPhysicsLinearVelocity(FVector::ZeroVector);
        BirdMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision); // 추가 충돌 방지
        BirdMesh->SetVisibility(false); // 폭발했으므로 새의 모습은 감춤
    }
    
    FVector ExplodeLocation = GetActorLocation(); 

    // 2. 폭발 시각 효과 (디버그 구체)
    DrawDebugSphere(GetWorld(), ExplodeLocation, ExplosionRadius, 32, FColor::Red, false, 2.0f, 0, 2.0f);

    // 3. 주변 물리 객체 탐색 및 충격 적용
    TArray<FHitResult> OutHits;
    FCollisionShape SphereShape = FCollisionShape::MakeSphere(ExplosionRadius);

    bool bHit = GetWorld()->SweepMultiByChannel(
        OutHits, 
        ExplodeLocation, 
        ExplodeLocation, 
        FQuat::Identity, 
        ECC_PhysicsBody, 
        SphereShape
    );

    if (bHit)
    {
        for (auto& Hit : OutHits)
        {
            UPrimitiveComponent* TargetComp = Hit.GetComponent();
            if (TargetComp && TargetComp->IsSimulatingPhysics())
            {
                float Distance = FVector::Dist(ExplodeLocation, TargetComp->GetComponentLocation());
                float DistanceRatio = FMath::Clamp(Distance / ExplosionRadius, 0.0f, 1.0f);
                float PowerAlpha = FMath::Lerp(1.0f, 0.7f, DistanceRatio);

                TargetComp->AddRadialImpulse(
                    ExplodeLocation, 
                    ExplosionRadius, 
                    ExplosionStrength * PowerAlpha, 
                    ERadialImpulseFalloff::RIF_Constant, 
                    true 
                );
            }
        }
    }

    // --- [핵심 수정 부분] ---
    // 4. 부모(Base_Bird)의 카메라 복귀 로직을 호출합니다.
    // 폭발의 여운을 보여주기 위해 1초 정도 뒤에 카메라를 돌립니다.
    GetWorldTimerManager().SetTimer(DespawnTimerHandle, this, &ABomb_Bird::StartCameraReturn, 1.0f, false);
    
    UE_LOG(LogTemp, Warning, TEXT("BombBird: 폭발 완료, 1초 후 카메라 복귀 시작"));
}
