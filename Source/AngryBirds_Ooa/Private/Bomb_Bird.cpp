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
    if (!BirdMesh) return;

    // 혹시 남아있을지 모를 타이머 제거
    GetWorldTimerManager().ClearTimer(BombTimerHandle);
    bAbilityUsed = true;

    FVector ExplodeLocation = BirdMesh->GetComponentLocation(); 

    // 1. 시각적 효과 (디버그 구체)
    DrawDebugSphere(GetWorld(), ExplodeLocation, ExplosionRadius, 32, FColor::Red, false, 2.0f, 0, 2.0f);

    // 2. 주변 물리 객체 탐색 및 충격 적용
    TArray<FHitResult> OutHits;
    FCollisionShape SphereShape = FCollisionShape::MakeSphere(ExplosionRadius);

    // 주변의 물리 객체(ECC_PhysicsBody)를 모두 찾습니다.
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
                // 거리 기반 위력 계산
                float Distance = FVector::Dist(ExplodeLocation, TargetComp->GetComponentLocation());
                float DistanceRatio = FMath::Clamp(Distance / ExplosionRadius, 0.0f, 1.0f);
                
                // 중심에서 멀어질수록 힘이 약해짐 (최대 100% ~ 최소 70%)
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

    // 3. 폭발 후 새의 처리
    BirdMesh->SetVisibility(false); 
    BirdMesh->SetSimulatePhysics(false); 
    BirdMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision); 

    // 3초 뒤 액터 완전히 제거 (Base_Bird의 DespawnTimerHandle과는 별개로 즉시 처리)
    FTimerHandle DestroyTimer;
    GetWorldTimerManager().SetTimer(DestroyTimer, [this]()
    {
        if (IsValid(this))
        {
            this->Destroy();
        }
    }, 2.0f, false);
}