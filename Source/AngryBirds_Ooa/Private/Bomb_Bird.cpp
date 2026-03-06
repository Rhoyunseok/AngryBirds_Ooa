#include "Bomb_Bird.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "DrawDebugHelpers.h"
#include "Camera/CameraShakeBase.h"
#include "GameFramework/SpringArmComponent.h"

ABomb_Bird::ABomb_Bird() : Super()
{
    PrimaryActorTick.bCanEverTick = true; 

    // [핵심] 물리 연산이 끝난 뒤에 Tick이 실행되도록 하여 강제 변형 경고를 방지합니다.
    PrimaryActorTick.TickGroup = TG_PostPhysics;

    ExplosionRadius = 900.0f;
    ExplosionStrength = 7000.0f;
    FuseDuration = 1.75f; 
    bIsIgnited = false;
}

void ABomb_Bird::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bIsIgnited && DynamicMat && BirdMesh)
    {
        CurrentFuseTime += DeltaTime;
        float Alpha = FMath::Clamp(CurrentFuseTime / FuseDuration, 0.0f, 1.0f);
        
        // 1. 머티리얼 업데이트 (색상)
        DynamicMat->SetScalarParameterValue(TEXT("HeatIntensity"), Alpha);

        // 2. 스케일 업데이트
        FVector CurrentScale = BirdMesh->GetComponentScale();
        float TargetScaleValue = 1.0f + (Alpha * Alpha * 0.35f); 
        FVector NewScale = FMath::VInterpTo(CurrentScale, FVector(TargetScaleValue), DeltaTime, 5.0f);
        
        if (!NewScale.Equals(CurrentScale, 0.005f))
        {
            // 시각적 스케일 설정
            BirdMesh->SetWorldScale3D(NewScale);
            
            // 물리 바디 스케일 강제 동기화 (경고 방지 핵심)
            if (BirdMesh->GetBodyInstance())
            {
                BirdMesh->GetBodyInstance()->UpdateBodyScale(NewScale);
            }
        }

        // 3. 카메라 추적 보정 (새가 느려질 때 카메라가 자연스럽게 뒤로 밀림)
        if (SpringArm) 
        {
            SpringArm->bEnableCameraLag = true;
            // 끝으로 갈수록 Lag 속도를 낮춰서(2.0f) 물리적 감속을 부드럽게 표현
            SpringArm->CameraLagSpeed = FMath::Lerp(12.0f, 2.0f, Alpha);
            
            float TargetArmLength = 800.0f + (Alpha * 450.0f);
            SpringArm->TargetArmLength = FMath::FInterpTo(SpringArm->TargetArmLength, TargetArmLength, DeltaTime, 2.0f);
            
            // 새가 커져서 화면을 가리지 않게 소켓 위치를 살짝 위로 이동
            SpringArm->SocketOffset = FMath::VInterpTo(SpringArm->SocketOffset, FVector(0, 0, Alpha * 120.0f), DeltaTime, 2.0f);
        }

        // 4. 진동 효과 (반드시 TeleportPhysics 플래그 사용)
        if (Alpha > 0.85f)
        {
            float ShakeIntensity = (Alpha - 0.85f) * 15.0f;
            FVector ShakeOffset = FVector(
                FMath::FRandRange(-1.f, 1.f), 
                FMath::FRandRange(-1.f, 1.f), 
                FMath::FRandRange(-1.f, 1.f)
            ) * ShakeIntensity;
            
            // [중요] ETeleportType::TeleportPhysics를 사용하여 로그 경고 차단
            BirdMesh->AddRelativeLocation(ShakeOffset, false, nullptr, ETeleportType::TeleportPhysics);
        }
    }
}

void ABomb_Bird::UseAbility()
{
    if (bAbilityUsed || !bHasLaunched) return;

    if (!bHasHitSomething)
    {
        UE_LOG(LogTemp, Log, TEXT("BombBird: 공중 능력 사용"));
        StartFuseSequence();
    }
}

void ABomb_Bird::OnBirdHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    Super::OnBirdHit(HitComponent, OtherActor, OtherComp, NormalImpulse, Hit);

    if (!bAbilityUsed)
    {
        StartFuseSequence();
    }
}

void ABomb_Bird::StartFuseSequence()
{
    if (bAbilityUsed && bIsIgnited) return; 

    bAbilityUsed = true; 
    bIsIgnited = true;
    CurrentFuseTime = 0.0f;

    if (BirdMesh)
    {
        // 굴러가야 하므로 물리 유지
        BirdMesh->SetSimulatePhysics(true);
        
        if (!DynamicMat)
        {
            DynamicMat = BirdMesh->CreateDynamicMaterialInstance(0);
        }
    }

    GetWorldTimerManager().SetTimer(BombTimerHandle, this, &ABomb_Bird::Explode, FuseDuration, false);
}

void ABomb_Bird::Explode()
{
    if (!BirdMesh) return;
    
    GetWorldTimerManager().ClearTimer(BombTimerHandle);
    bIsIgnited = false;

    // [중요] 진동 오프셋 초기화 시에도 TeleportPhysics 플래그를 넣어야 로그가 안 뜹니다.
    BirdMesh->SetRelativeLocation(FVector::ZeroVector, false, nullptr, ETeleportType::TeleportPhysics);

    // 현재 실시간 위치 확보
    FVector RealWorldLocation = BirdMesh->GetComponentLocation();

    // 1. 사운드 및 이펙트
    if (AbilityVoiceSound) 
        UGameplayStatics::PlaySoundAtLocation(this, AbilityVoiceSound, RealWorldLocation);

    if (ExplosionParticle)
        UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionParticle, RealWorldLocation, FRotator::ZeroRotator, FVector(3.5f));

    // 2. 카메라 흔들림
    if (ExplosionCameraShake)
    {
        APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
        if (PC) PC->ClientStartCameraShake(ExplosionCameraShake, 100.0f);
    }

    // 3. 물리 중단 및 숨기기
    BirdMesh->SetSimulatePhysics(false);
    BirdMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    BirdMesh->SetVisibility(false); 

    // 4. 범위 물리 충격 및 데미지 적용
    TArray<FHitResult> OutHits;
    FCollisionShape SphereShape = FCollisionShape::MakeSphere(ExplosionRadius);
    
    if (GetWorld()->SweepMultiByChannel(OutHits, RealWorldLocation, RealWorldLocation, FQuat::Identity, ECC_PhysicsBody, SphereShape))
    {
        for (auto& Hit : OutHits)
        {
            if (UPrimitiveComponent* TargetComp = Hit.GetComponent())
            {
                TargetComp->AddRadialImpulse(RealWorldLocation, ExplosionRadius, ExplosionStrength, ERadialImpulseFalloff::RIF_Constant, true);
            }
            
            // 블럭에 데미지 부여
            AActor* HitActor = Hit.GetActor();
            UGameplayStatics::ApplyDamage(HitActor,1000.0f, GetInstigatorController(), this, UDamageType::StaticClass());
        }
    }

    // 디버그 구체 시각화
    DrawDebugSphere(GetWorld(), RealWorldLocation, ExplosionRadius, 32, FColor::Red, false, 2.0f, 0, 1.5f);

    // 카메라 리턴 시퀀스
    GetWorldTimerManager().SetTimer(DespawnTimerHandle, this, &ABase_Bird::StartCameraReturn, 0.5f, false);
}