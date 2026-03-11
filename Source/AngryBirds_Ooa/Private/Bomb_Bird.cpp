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

    // 물리가 켜져 있고 굴러가는 중이라면 루트(액터)를 메쉬 위치로 강제 이동
    if (BirdMesh && BirdMesh->IsSimulatingPhysics())
    {
        FVector MeshLocation = BirdMesh->GetComponentLocation();
        
        // 부모(Actor)를 자식(Mesh)의 위치로 옮깁니다. 
        // 이때 bTeleport를 true로 주어 물리 연산에 꼬임이 없게 합니다.
        SetActorLocation(MeshLocation, false, nullptr, ETeleportType::TeleportPhysics);
        
        // 루트가 메쉬 위치로 왔으므로, 메쉬의 상대 위치는 다시 (0,0,0)이 되어야 함
        BirdMesh->SetRelativeLocation(FVector::ZeroVector);
    }
    
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
    // 공중에서 클릭 시: 도화선(지연 폭발) 시작
    if (bAbilityUsed || !bHasLaunched) return;
    
    StartFuseSequence();
}

void ABomb_Bird::OnBirdHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    // 부모 클래스의 히트 처리 (기본 사운드 등)
    Super::OnBirdHit(HitComponent, OtherActor, OtherComp, NormalImpulse, Hit);

    // [수정] 충돌하면 도화선 상태든 아니든 즉시 폭발 호출
    Explode();
}

void ABomb_Bird::StartFuseSequence()
{
    if (bIsIgnited) return; 

    bAbilityUsed = true; 
    bIsIgnited = true;
    CurrentFuseTime = 0.0f;

    if (BirdMesh)
    {
        // 도화선 중에도 물리적으로 움직여야 함
        BirdMesh->SetSimulatePhysics(true);
        if (!DynamicMat)
        {
            DynamicMat = BirdMesh->CreateDynamicMaterialInstance(0);
        }
    }

    // FuseDuration 후에 Explode 호출하도록 예약
    GetWorldTimerManager().SetTimer(BombTimerHandle, this, &ABomb_Bird::Explode, FuseDuration, false);
}

void ABomb_Bird::Explode()
{
    // [중요] 중복 실행 방지: 타이머가 살아있다면 제거 (충돌 즉시 폭발 시 필요)
    if (GetWorldTimerManager().IsTimerActive(BombTimerHandle))
    {
        GetWorldTimerManager().ClearTimer(BombTimerHandle);
    }

    // 이미 터져서 사라진 상태라면 무시
    if (!BirdMesh || !BirdMesh->IsVisible()) return;
    
    bIsIgnited = false;
    bAbilityUsed = true; // 버튼 안 누르고 충돌만 했어도 사용 처리

    // 1. 위치 확정: 현재 물리로 굴러간 메쉬 위치를 사용
    FVector ExplosionLocation = BirdMesh->GetComponentLocation();

    // 2. 물리 및 시각 초기화
    BirdMesh->SetRelativeLocation(FVector::ZeroVector, false, nullptr, ETeleportType::TeleportPhysics);
    BirdMesh->SetSimulatePhysics(false);
    BirdMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    BirdMesh->SetVisibility(false); 

    // 3. 액터 위치 동기화 (카메라 및 디버그용)
    SetActorLocation(ExplosionLocation);

    // 4. 사운드 및 이펙트 발생
    if (AbilityVoiceSound) 
        UGameplayStatics::PlaySoundAtLocation(this, AbilityVoiceSound, ExplosionLocation);

    if (ExplosionParticle)
        UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionParticle, ExplosionLocation, FRotator::ZeroRotator, FVector(3.5f));

    // 5. 카메라 흔들림
    if (ExplosionCameraShake)
    {
        APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
        if (PC) PC->ClientStartCameraShake(ExplosionCameraShake, 100.0f);
    }

    // 6. 범위 물리 충격 및 데미지 적용
    TArray<FOverlapResult> OverlapResults;
    FCollisionShape SphereShape = FCollisionShape::MakeSphere(ExplosionRadius);
    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(this);

    if (GetWorld()->OverlapMultiByChannel(OverlapResults, ExplosionLocation, FQuat::Identity, ECC_PhysicsBody, SphereShape, QueryParams))
    {
        for (auto& Result : OverlapResults)
        {
            if (UPrimitiveComponent* TargetComp = Result.GetComponent())
            {
                TargetComp->AddRadialImpulse(ExplosionLocation, ExplosionRadius, ExplosionStrength, ERadialImpulseFalloff::RIF_Constant, true);
                
                AActor* HitActor = Result.GetActor();
                if (HitActor)
                {
                    UGameplayStatics::ApplyDamage(HitActor, 1000.0f, GetInstigatorController(), this, UDamageType::StaticClass());
                }
            }
        }
    }

    // 디버그 구체 (선택)
    // DrawDebugSphere(GetWorld(), ExplosionLocation, ExplosionRadius, 32, FColor::Red, false, 2.0f);

    // 7. 카메라 리턴 (Base_Bird의 로직 호출)
    GetWorldTimerManager().SetTimer(DespawnTimerHandle, this, &ABase_Bird::StartCameraReturn, 1.5f, false);
}