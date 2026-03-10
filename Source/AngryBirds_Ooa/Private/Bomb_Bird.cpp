#include "Bomb_Bird.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "DrawDebugHelpers.h"
#include "Camera/CameraShakeBase.h"
#include "Engine/OverlapResult.h"
#include "GameFramework/SpringArmComponent.h"

ABomb_Bird::ABomb_Bird() : Super()
{
    PrimaryActorTick.bCanEverTick = true; 
    // 물리 연산 후 Tick 실행 (스케일 변경 시 경고 방지)
    PrimaryActorTick.TickGroup = TG_PostPhysics;

    ExplosionRadius = 900.0f;
    ExplosionStrength = 7000.0f;
    FuseDuration = 1.75f; 
    bIsIgnited = false;
}

void ABomb_Bird::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bIsIgnited && BirdMesh)
    {
        CurrentFuseTime += DeltaTime;
        float Alpha = FMath::Clamp(CurrentFuseTime / FuseDuration, 0.0f, 1.0f);
        
        // 1. 머티리얼 업데이트
        if (DynamicMat)
        {
            DynamicMat->SetScalarParameterValue(TEXT("HeatIntensity"), Alpha);
        }

        // 2. 스케일 업데이트 (시각적 부풀기)
        FVector CurrentScale = BirdMesh->GetRelativeScale3D();
        float TargetScaleValue = 0.5f + (Alpha * Alpha * 0.35f); // 초기 스케일 0.5 기준 (사진 참고)
        FVector NewScale = FMath::VInterpTo(CurrentScale, FVector(TargetScaleValue), DeltaTime, 5.0f);
        
        if (!NewScale.Equals(CurrentScale, 0.005f))
        {
            BirdMesh->SetRelativeScale3D(NewScale);
            // 물리 바디 스케일 동기화
            if (BirdMesh->GetBodyInstance())
            {
                BirdMesh->GetBodyInstance()->UpdateBodyScale(NewScale);
            }
        }

        // 3. 카메라 추적 보정 (SpringArm이 루트나 부모에 붙어있다고 가정)
        if (SpringArm) 
        {
            SpringArm->bEnableCameraLag = true;
            SpringArm->CameraLagSpeed = FMath::Lerp(12.0f, 2.0f, Alpha);
            
            float TargetArmLength = 800.0f + (Alpha * 450.0f);
            SpringArm->TargetArmLength = FMath::FInterpTo(SpringArm->TargetArmLength, TargetArmLength, DeltaTime, 2.0f);
            SpringArm->SocketOffset = FMath::VInterpTo(SpringArm->SocketOffset, FVector(0, 0, Alpha * 120.0f), DeltaTime, 2.0f);
        }

        // 4. 진동 효과 (TeleportPhysics를 사용하여 물리 연산 위치에 영향 주지 않음)
        if (Alpha > 0.85f)
        {
            float ShakeIntensity = (Alpha - 0.85f) * 15.0f;
            FVector ShakeOffset = FVector(
                FMath::FRandRange(-1.f, 1.f), 
                FMath::FRandRange(-1.f, 1.f), 
                FMath::FRandRange(-1.f, 1.f)
            ) * ShakeIntensity;
            
            BirdMesh->SetRelativeLocation(ShakeOffset, false, nullptr, ETeleportType::TeleportPhysics);
        }
    }
}

void ABomb_Bird::UseAbility()
{
    // 공중에서 클릭 시 즉시 도화선 시작
    if (bAbilityUsed || !bHasLaunched) return;
    StartFuseSequence();
}

void ABomb_Bird::OnBirdHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    Super::OnBirdHit(HitComponent, OtherActor, OtherComp, NormalImpulse, Hit);

    // 충돌 시 도화선 시작 (이미 시작되었다면 무시)
    if (!bIsIgnited)
    {
        StartFuseSequence();
    }
}

void ABomb_Bird::StartFuseSequence()
{
    if (bIsIgnited) return; 

    bAbilityUsed = true; 
    bIsIgnited = true;
    CurrentFuseTime = 0.0f;

    if (BirdMesh)
    {
        // 충돌 후에도 계속 굴러가야 하므로 물리 유지
        BirdMesh->SetSimulatePhysics(true);
        if (!DynamicMat)
        {
            DynamicMat = BirdMesh->CreateDynamicMaterialInstance(0);
        }
    }

    // FuseDuration(1.75초) 후에 폭발
    GetWorldTimerManager().SetTimer(BombTimerHandle, this, &ABomb_Bird::Explode, FuseDuration, false);
}

void ABomb_Bird::Explode()
{
    if (!BirdMesh) return;
    
    GetWorldTimerManager().ClearTimer(BombTimerHandle);
    bIsIgnited = false;

    // [핵심] 진동 오프셋 초기화 (자식인 메시를 부모 위치로 복귀)
    BirdMesh->SetRelativeLocation(FVector::ZeroVector, false, nullptr, ETeleportType::TeleportPhysics);

    // [핵심] 폭발 위치를 '액터'의 월드 위치로 설정 (충돌 후 굴러간 최종 위치)
    FVector ExplosionLocation = GetActorLocation();

    // 1. 사운드 및 이펙트
    if (AbilityVoiceSound) 
        UGameplayStatics::PlaySoundAtLocation(this, AbilityVoiceSound, ExplosionLocation);

    if (ExplosionParticle)
        UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionParticle, ExplosionLocation, FRotator::ZeroRotator, FVector(3.5f));

    // 2. 카메라 흔들림
    if (ExplosionCameraShake)
    {
        APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
        if (PC) PC->ClientStartCameraShake(ExplosionCameraShake, 100.0f);
    }

    // 3. 물리 중단 및 시각적 제거
    BirdMesh->SetSimulatePhysics(false);
    BirdMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    BirdMesh->SetVisibility(false); 

    // 4. 범위 물리 충격 및 데미지 적용
    TArray<FOverlapResult> OverlapResults;
    FCollisionShape SphereShape = FCollisionShape::MakeSphere(ExplosionRadius);
    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(this); // 본인 제외

    // OverlapMulti를 사용하여 범위 내 모든 컴포넌트 검출
    if (GetWorld()->OverlapMultiByChannel(OverlapResults, ExplosionLocation, FQuat::Identity, ECC_PhysicsBody, SphereShape, QueryParams))
    {
        for (auto& Result : OverlapResults)
        {
            if (UPrimitiveComponent* TargetComp = Result.GetComponent())
            {
                // 충격 가하기
                TargetComp->AddRadialImpulse(ExplosionLocation, ExplosionRadius, ExplosionStrength, ERadialImpulseFalloff::RIF_Constant, true);
                
                // 데미지 가하기
                AActor* HitActor = Result.GetActor();
                if (HitActor)
                {
                    UGameplayStatics::ApplyDamage(HitActor, 1000.0f, GetInstigatorController(), this, UDamageType::StaticClass());
                }
            }
        }
    }

    // 디버그 구체
    DrawDebugSphere(GetWorld(), ExplosionLocation, ExplosionRadius, 32, FColor::Red, false, 2.0f, 0, 1.5f);

    // 카메라 리턴 (Base_Bird의 로직 호출)
    GetWorldTimerManager().SetTimer(DespawnTimerHandle, this, &ABase_Bird::StartCameraReturn, 0.5f, false);
}