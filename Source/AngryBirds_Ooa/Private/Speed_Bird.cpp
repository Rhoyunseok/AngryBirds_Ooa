#include "Speed_Bird.h"
#include "Components/SkeletalMeshComponent.h"

ASpeed_Bird::ASpeed_Bird() : Super()
{
	// 대시 파워는 기존 속도에 더해질 값이므로 프로젝트에 따라 조절 (예: 2000~4000)
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

	// 3. 시각적 피드백 (선택 사항: 빨라질 때 새의 각도를 속도 방향으로 즉시 정렬)
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
		// 0.5~1.0 정도가 자연스럽게 굴러가는 값입니다. (15.0은 너무 높음)
		BirdMesh->SetLinearDamping(0.8f); 
		BirdMesh->SetAngularDamping(0.5f);
        
		// 부딪혔을 때 스피드버드답게 좀 더 강하게 튕겨나가게 하고 싶다면 
		// 부모에서 설정된 속도를 여기서 조금 더 보정할 수 있습니다.
		// (현재는 부모의 로직이 이미 훌륭하므로 굳이 속도를 20%로 줄일 필요가 없습니다.)
	}
}