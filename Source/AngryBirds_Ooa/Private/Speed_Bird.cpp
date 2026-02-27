#include "Speed_Bird.h"
#include "Components/SkeletalMeshComponent.h"

ASpeed_Bird::ASpeed_Bird() : Super()
{
	// 대시 파워는 기존 속도에 더해질 값이므로 조절
	DashPower = 3000.0f; 
}

void ASpeed_Bird::UseAbility()
{
	// 1. 이미 능력을 썼거나 부딪혔으면 무시
	if (bAbilityUsed || bHasHitSomething || !bHasLaunched) return;

	// 2. 현재 이동 방향(CustomVelocity)을 기준으로 가속
	if (CustomVelocity.IsNearlyZero())
	{
		CustomVelocity = GetActorForwardVector() * DashPower;
	}
	else
	{
		// 현재 진행 방향으로 속도를 크게 증가시킴
		FVector DashDirection = CustomVelocity.GetSafeNormal();
		CustomVelocity = DashDirection * (CustomVelocity.Size() + DashPower);
	}

	// 3. 시각적 피드백
	SetActorRotation(CustomVelocity.Rotation());

	bAbilityUsed = true;
	UE_LOG(LogTemp, Warning, TEXT("Speed Bird: Dash! New Velocity Size: %f"), CustomVelocity.Size());
}

void ASpeed_Bird::OnBirdHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// 부모의 OnBirdHit를 호출하여 SimulatePhysics를 켜고 CustomVelocity를 전달함
	Super::OnBirdHit(HitComponent, OtherActor, OtherComp, NormalImpulse, Hit);

	if (BirdMesh)
	{
		// 부딪힌 후 너무 떡처럼 붙지 않게 Damping 값을 적절히 조절
		BirdMesh->SetLinearDamping(0.8f); 
		BirdMesh->SetAngularDamping(0.5f);
	}
}