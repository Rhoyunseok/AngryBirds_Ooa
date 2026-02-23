#include "Bomb_Bird.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "DrawDebugHelpers.h" // 디버그 드로잉을 위해 필수!

ABomb_Bird::ABomb_Bird()
{
    ExplosionRadius = 900.0f;
    ExplosionStrength = 7000.0f;
    // 초기화 시 bAbilityUsed는 기본적으로 false여야 합니다 (Base_Bird 확인 필요)
}

void ABomb_Bird::UseAbility()
{
    // 이미 능력을 사용했거나 아직 발사 전이면 무시
    if (bAbilityUsed || !bHasLaunched) return;

    // 비행 중 클릭 시 즉시 폭발
    if (!bHasHitSomething)
    {
        UE_LOG(LogTemp, Warning, TEXT("BombBird: [능력사용] 공중 폭발!"));
        Explode();
    }
}

void ABomb_Bird::OnBirdHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    Super::OnBirdHit(HitComponent, OtherActor, OtherComp, NormalImpulse, Hit);

    // 충돌 시 능력을 사용하지 않았다면 1초 뒤 자동 폭발 예약
    if (!bAbilityUsed)
    {
        bAbilityUsed = true; // 여기서 true로 만들어 중복 실행 방지
        UE_LOG(LogTemp, Log, TEXT("BombBird: [충돌] 1초 후 폭발 예약"));

        GetWorldTimerManager().SetTimer(BombTimerHandle, this, &ABomb_Bird::Explode, 1.0f, false);
    }
}

// -------------------------------------------------------------
// [추가 로직] 드래그 중 폭발 예상 범위를 실시간으로 표시
// -------------------------------------------------------------
void ABomb_Bird::DisplayTrajectory()
{
    // 부모의 기본적인 궤적 표시 로직(포물선)을 먼저 실행
    Super::DisplayTrajectory();

    // 조준 중일 때만 예상 범위를 그립니다.
    if (bIsDragging)
    {
        APlayerController* PC = Cast<APlayerController>(GetWorld()->GetFirstPlayerController());
        FVector2D CurrentMouse;
        if (PC && PC->GetMousePosition(CurrentMouse.X, CurrentMouse.Y))
        {
            FVector2D DragDelta = CurrentMouse - StartMousePos;

            // Base_Bird와 동일한 물리 발사 공식 적용
            float PitchAngle = FMath::Clamp(((FMath::Abs(DragDelta.Y) - FMath::Abs(DragDelta.X)) / MaxDragDist) * 90.0f, 0.0f, 90.0f);
            float YawAngle = FMath::Clamp((-DragDelta.X / MaxDragDist) * 45.0f, -45.0f, 45.0f);
            FVector LaunchDir = FRotator(PitchAngle, YawAngle, 0.f).Vector();
            float Power = FMath::Clamp(DragDelta.Size() / MaxDragDist, 0.0f, 1.0f) * 2500.0f;
            FVector LaunchVelocity = LaunchDir * Power;

            FVector StartPos = GetActorLocation() + (LaunchDir * 200.0f);

            // 물리 시뮬레이션으로 충돌 예상 지점 계산
            FPredictProjectilePathParams PathParams(15.0f, StartPos, LaunchVelocity, 3.0f);
            PathParams.bTraceWithChannel = true;
            PathParams.TraceChannel = ECollisionChannel::ECC_WorldStatic;

            FPredictProjectilePathResult PathResult;
            if (UGameplayStatics::PredictProjectilePath(this, PathParams, PathResult))
            {
                // 궤적이 끝나는 지점(지면 혹은 벽면)을 획득
                FVector ImpactPoint = PathResult.HitResult.Location;

                // 예상 폭발 지점에 투명한 빨간색 구체를 그려 범위를 표시 (매 프레임 갱신)
                DrawDebugSphere(GetWorld(), ImpactPoint, ExplosionRadius, 16, FColor(255, 0, 0, 100), false, -1.0f, 0, 1.5f);
                
                // 중심점에 작은 십자선 표시
                DrawDebugPoint(GetWorld(), ImpactPoint, 10.0f, FColor::Red, false, -1.0f);
            }
        }
    }
}

void ABomb_Bird::Explode()
{
    if (!BirdMesh) return;

    UE_LOG(LogTemp, Error, TEXT("BombBird: [실행] 펑!!!"));
    GetWorldTimerManager().ClearTimer(BombTimerHandle);
    bAbilityUsed = true;

    // [수정 포인트] GetActorLocation() 대신 BirdMesh의 현재 월드 위치를 사용합니다.
    FVector ExplodeLocation = BirdMesh->GetComponentLocation(); 

    // 디버그 로그로 현재 폭발 좌표가 어디인지 찍어보세요.
    UE_LOG(LogTemp, Warning, TEXT("BombBird: 폭발 발생 좌표 -> %s"), *ExplodeLocation.ToString());

    // 1. 디버그 시각화 (빨간색 구체)
    DrawDebugSphere(GetWorld(), ExplodeLocation, ExplosionRadius, 32, FColor::Red, false, 2.0f, 0, 2.0f);

    // 2. 360도 범위 물리 충격
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
                // --- 커스텀 거리 기반 위력 계산 추가 ---
                
                // 1. 물체와의 거리 계산
                float Distance = FVector::Dist(ExplodeLocation, TargetComp->GetComponentLocation());
                
                // 2. 거리 비율 (0.0: 중심, 1.0: 범위 끝)
                float DistanceRatio = FMath::Clamp(Distance / ExplosionRadius, 0.0f, 1.0f);

                // 3. 위력 보간 (중심에선 100%, 끝에선 최소 20%의 힘을 유지)
                // 만약 끝에서 더 세게 밀고 싶다면 0.2f를 0.4f 등으로 올리세요.
                float PowerAlpha = FMath::Lerp(1.0f, 0.7f, DistanceRatio);

                // 4. 계산된 위력으로 충격 가하기
                // RIF_Constant를 사용해야 우리가 직접 계산한 PowerAlpha가 정확히 적용됩니다.
                TargetComp->AddRadialImpulse(
                    ExplodeLocation, 
                    ExplosionRadius, 
                    ExplosionStrength * PowerAlpha, 
                    ERadialImpulseFalloff::RIF_Constant, 
                    true 
                );

                UE_LOG(LogTemp, Log, TEXT("Target: %s, 위력 배율: %.2f"), *TargetComp->GetName(), PowerAlpha);
            }
        }
    }

    // (이후 카메라 고정 및 파괴 로직은 동일...)
    BirdMesh->SetVisibility(false); 
    BirdMesh->SetSimulatePhysics(false); 
    BirdMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision); 

    FTimerHandle DestroyTimer;
    GetWorldTimerManager().SetTimer(DestroyTimer, [this]()
    {
        if (IsValid(this))
        {
            this->Destroy();
        }
    }, 3.0f, false);
}