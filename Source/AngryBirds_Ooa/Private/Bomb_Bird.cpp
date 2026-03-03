#include "Bomb_Bird.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "DrawDebugHelpers.h"

ABomb_Bird::ABomb_Bird() : Super()
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
    Super::OnBirdHit(HitComponent, OtherActor, OtherComp, NormalImpulse, Hit);

    if (!bAbilityUsed)
    {
        bAbilityUsed = true; 
        UE_LOG(LogTemp, Log, TEXT("BombBird: 충돌 확인, 1초 후 지연 폭발 예약"));

        // 1초 뒤에 지연 폭발 실행 (이 1초 동안 새가 데굴데굴 구릅니다)
        GetWorldTimerManager().SetTimer(BombTimerHandle, this, &ABomb_Bird::Explode, 1.0f, false);
    }
}

void ABomb_Bird::Explode()
{
    if (!BirdMesh) return;
    GetWorldTimerManager().ClearTimer(BombTimerHandle);
    bAbilityUsed = true;

    // [중요] 물리 엔진에 의해 따로 놀고 있는 Mesh의 "현재 실제 월드 위치"를 먼저 가져옵니다.
    FVector RealWorldLocation = BirdMesh->GetComponentLocation();

    // 1. 사운드 재생
    if (AbilityVoiceSound) 
    { 
        // 사운드도 메쉬가 있는 위치에서 들리도록 재생
        UGameplayStatics::PlaySoundAtLocation(this, AbilityVoiceSound, RealWorldLocation);
    }

    // 2. 능력 전용 폭발 파티클(Cascade) 스폰
    if (ExplosionParticle)
    {
        UGameplayStatics::SpawnEmitterAtLocation(
            GetWorld(),
            ExplosionParticle,
            RealWorldLocation, // [수정] GetActorLocation() 대신 메쉬가 굴러간 위치 사용!
            FRotator::ZeroRotator,
            FVector(3.5f) 
        );
    }

    // --- 폭발 물리 로직 ---
    
    // 3. 액터의 루트를 메쉬가 있는 곳으로 강제 이동 (카메라와 물리 충격 기준점을 맞춤)
    SetActorLocation(RealWorldLocation, false, nullptr, ETeleportType::TeleportPhysics);

    bUseCustomPhysics = false; 
    CustomVelocity = FVector::ZeroVector; 
    
    // 물리 중단 및 외형 숨기기
    BirdMesh->SetSimulatePhysics(false);
    BirdMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    BirdMesh->SetVisibility(false); 

    // 주변 물리 객체 탐색 및 충격 적용
    TArray<FHitResult> OutHits;
    FCollisionShape SphereShape = FCollisionShape::MakeSphere(ExplosionRadius);

    // 실제 메쉬 위치(RealWorldLocation) 기준으로 폭발 범위 체크
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
                TargetComp->AddRadialImpulse(
                    RealWorldLocation, 
                    ExplosionRadius, 
                    ExplosionStrength, 
                    ERadialImpulseFalloff::RIF_Constant, 
                    true
                );
            }
            
            if (Hit.GetActor())
            {
                UGameplayStatics::ApplyRadialDamage(
                    this, 100.0f, RealWorldLocation, ExplosionRadius, 
                    UDamageType::StaticClass(), TArray<AActor*>(), this
                );
            }
        }
    }

    // 디버그 구체 표시 (위치 확인용)
    DrawDebugSphere(GetWorld(), RealWorldLocation, ExplosionRadius, 32, FColor::Red, false, 2.0f, 0, 1.5f);

    // 1초 뒤에 카메라를 돌립니다.
    GetWorldTimerManager().SetTimer(DespawnTimerHandle, this, &ABase_Bird::StartCameraReturn, 1.0f, false);
}