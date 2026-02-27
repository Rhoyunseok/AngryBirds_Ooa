// Fill out your copyright notice in the Description page of Project Settings.


#include "PYB/TNTBlock.h"

#include "Kismet/GameplayStatics.h"

// Sets default values
ATNTBlock::ATNTBlock()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ExplosionRadius = 500.0f;
	ExplosionStrength = 7000.0f;
}

// Called when the game starts or when spawned
void ATNTBlock::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATNTBlock::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATNTBlock::Explode()
{
	FVector ExplodeLocation = GetActorLocation();
	
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
				
				AActor* HitActor = Hit.GetActor();
				if (HitActor)
				{
					UGameplayStatics::ApplyRadialDamage(
					this, 
					100, 
					ExplodeLocation, 
					ExplosionRadius, 
					UDamageType::StaticClass(), 
					TArray<AActor*>(), 
					this
					);
				}

				UE_LOG(LogTemp, Log, TEXT("Target: %s, 위력 배율: %.2f"), *TargetComp->GetName(), PowerAlpha);
			}
		}
	}
}
