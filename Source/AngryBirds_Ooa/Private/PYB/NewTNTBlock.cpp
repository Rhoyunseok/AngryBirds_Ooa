// Fill out your copyright notice in the Description page of Project Settings.


#include "PYB/NewTNTBlock.h"
#include "Kismet/GameplayStatics.h"


ANewTNTBlock::ANewTNTBlock()
{
	BlockHP = 1.0f;
	BlockThreshold = 200.0f;
	BlockPrice = 300.0f;
	ExplosionRadius = 500.0f * (GetActorScale3D().Size() * 3);
}

void ANewTNTBlock::BeginPlay()
{
	Super::BeginPlay();
}

void ANewTNTBlock::BeforeBlockDestory()
{
	Super::BeforeBlockDestory();
	UE_LOG(LogTemp, Warning, TEXT("TNT 펑"));
	Explode();
}

void ANewTNTBlock::Explode()
{
	FVector ExplodeLocation = GetActorLocation();
	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(this);
	
	DrawDebugSphere(GetWorld(), ExplodeLocation, ExplosionRadius, 32, FColor::Red, false, 0.5f, 0, 2.0f);

	// 2. 360도 범위 물리 충격
	TArray<FHitResult> OutHits;
	FCollisionShape SphereShape = FCollisionShape::MakeSphere(ExplosionRadius);
	
	bool bHit = GetWorld()->SweepMultiByChannel(
		OutHits, 
		ExplodeLocation, 
		ExplodeLocation, 
		FQuat::Identity, 
		ECC_Visibility, 
		SphereShape
	);

	if (bHit)
	{
		for (auto& Hit : OutHits)
		{
			AActor* HitActor = Hit.GetActor();
			if (!HitActor || HitActor == this) continue;
			
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
			
			UGameplayStatics::ApplyDamage(
				HitActor,
				1000.0f,
				GetInstigatorController(),
				this,
				UDamageType::StaticClass()
				);
			
		}
	}
}