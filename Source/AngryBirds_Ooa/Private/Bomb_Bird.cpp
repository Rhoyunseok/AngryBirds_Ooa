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
    if (bAbilityUsed || !bHasLaunched) return;

    if (!bHasHitSomething)
    {
        UE_LOG(LogTemp, Warning, TEXT("BombBird: 공중 폭발 실행!"));
        Explode();
    }
}

void ABomb_Bird::OnBirdHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    // 부모 클래스의 OnBirdHit에서 물리 시뮬레이션이 켜집니다.
    Super::OnBirdHit(HitComponent, OtherActor, OtherComp, NormalImpulse, Hit);

    if (!bAbilityUsed)
    {
        bAbilityUsed = true; 
        UE_LOG(LogTemp, Log, TEXT("BombBird: 충돌 확인, 1초 후 지연 폭발 예약"));

        // 1초 뒤에 현재 굴러가있는 위치를 체크하여 터뜨립니다.
        GetWorldTimerManager().SetTimer(BombTimerHandle, this, &ABomb_Bird::Explode, 1.0f, false);
    }
}

void ABomb_Bird::Explode()
{
    // 1. 안전 검사 및 중복 실행 방지
    if (!BirdMesh) return;
    GetWorldTimerManager().ClearTimer(BombTimerHandle);
    bAbilityUsed = true;

    // ★ [핵심] 물리 엔진에 의해 따로 놀고 있는 Mesh의 실제 월드 위치를 가져옵니다.
    FVector RealWorldLocation = BirdMesh->GetComponentLocation();
    
    // 디버그 로그로 실제 위치 확인
    UE_LOG(LogTemp, Error, TEXT("BombBird: Explode 호출됨! 실제 메쉬 위치: %s"), *RealWorldLocation.ToString());

    // 2. 액터의 루트를 메쉬가 있는 곳으로 강제 이동
    // 이렇게 해야 DrawDebugSphere와 카메라 시스템이 정확한 위치를 잡습니다.
    SetActorLocation(RealWorldLocation, false, nullptr, ETeleportType::TeleportPhysics);

    // 3. 물리 중단 및 외형 숨기기
    bUseCustomPhysics = false; 
    CustomVelocity = FVector::ZeroVector; 
    
    BirdMesh->SetSimulatePhysics(false);
    BirdMesh->SetAllPhysicsLinearVelocity(FVector::ZeroVector);
    BirdMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    BirdMesh->SetVisibility(false); 

    // 4. 시각적 효과 (디버그 구체) - 이제 SetActorLocation을 했으므로 GetActorLocation()과 동일합니다.
    DrawDebugSphere(GetWorld(), RealWorldLocation, ExplosionRadius, 32, FColor::Red, false, 2.0f, 0, 2.0f);

    // 5. 주변 물리 객체 탐색 및 충격 적용
    TArray<FHitResult> OutHits;
    FCollisionShape SphereShape = FCollisionShape::MakeSphere(ExplosionRadius);

    // 실제 메쉬 위치(RealWorldLocation) 기준으로 Sweep 수행
    bool bHit = GetWorld()->SweepMultiByChannel(
        OutHits, 
        RealWorldLocation, 
        RealWorldLocation, 
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
                float Distance = FVector::Dist(RealWorldLocation, TargetComp->GetComponentLocation());
                float DistanceRatio = FMath::Clamp(Distance / ExplosionRadius, 0.0f, 1.0f);
                float PowerAlpha = FMath::Lerp(1.0f, 0.7f, DistanceRatio);

                TargetComp->AddRadialImpulse(
                    RealWorldLocation, 
                    ExplosionRadius, 
                    ExplosionStrength * PowerAlpha, 
                    ERadialImpulseFalloff::RIF_Constant, 
                    true 
                );
            }
        }
    }

    // 6. 카메라 복귀 호출 (Base_Bird의 로직 사용)
    // 1초 뒤에 카메라를 돌립니다.
    GetWorldTimerManager().SetTimer(DespawnTimerHandle, this, &ABomb_Bird::StartCameraReturn, 1.0f, false);
}