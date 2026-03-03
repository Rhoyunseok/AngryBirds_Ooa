#include "Bomb_Bird.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "DrawDebugHelpers.h"
#include "Camera/CameraShakeBase.h" // 카메라 쉐이크를 위해 추가

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

        // 1초 동안 새가 굴러가는 시간을 벌어줍니다.
        GetWorldTimerManager().SetTimer(BombTimerHandle, this, &ABomb_Bird::Explode, 1.0f, false);
    }
}

void ABomb_Bird::Explode()
{
    if (!BirdMesh) return;
    GetWorldTimerManager().ClearTimer(BombTimerHandle);
    bAbilityUsed = true;

    // 현재 물리적으로 굴러가 있는 메쉬의 위치를 가져옵니다.
    FVector RealWorldLocation = BirdMesh->GetComponentLocation();

    // 1. 사운드 재생
    if (AbilityVoiceSound) 
    { 
        UGameplayStatics::PlaySoundAtLocation(this, AbilityVoiceSound, RealWorldLocation);
    }

    // 2. 폭발 파티클 스폰
    if (ExplosionParticle)
    {
        UGameplayStatics::SpawnEmitterAtLocation(
            GetWorld(),
            ExplosionParticle,
            RealWorldLocation, 
            FRotator::ZeroRotator,
            FVector(3.5f) 
        );
    }

    // 3. [수정] 카메라 흔들림 효과 적용
    // 기존에 카메라를 워프시키던 SetActorLocation을 제거하고 이 로직을 넣습니다.
    if (ExplosionCameraShake)
    {
        APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
        if (PC && PC->PlayerCameraManager)
        {
            // 1.0f는 강도 배율입니다. 2.0f 등으로 높이면 더 격렬해집니다.
            PC->ClientStartCameraShake(ExplosionCameraShake, 100.0f);
        }
    }

    // 4. 물리 중단 및 외형 숨기기
    // 액터 자체는 이동시키지 않으므로 카메라 시점은 급격히 변하지 않습니다.
    BirdMesh->SetSimulatePhysics(false);
    BirdMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    BirdMesh->SetVisibility(false); 

    // 5. 주변 물리 객체 탐색 및 충격 적용
    TArray<FHitResult> OutHits;
    FCollisionShape SphereShape = FCollisionShape::MakeSphere(ExplosionRadius);

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

    // 디버그 구체 (테스트 후 주석 처리 가능)
    DrawDebugSphere(GetWorld(), RealWorldLocation, ExplosionRadius, 32, FColor::Red, false, 2.0f, 0, 1.5f);

    // 1초 뒤에 카메라를 원래 발사대로 돌립니다.
    GetWorldTimerManager().SetTimer(DespawnTimerHandle, this, &ABase_Bird::StartCameraReturn, 0.5f, false);
}