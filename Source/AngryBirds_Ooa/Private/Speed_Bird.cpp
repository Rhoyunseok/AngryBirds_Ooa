#include "Speed_Bird.h"
#include "Components/SkeletalMeshComponent.h"

ASpeed_Bird::ASpeed_Bird()
{
	// 대시 파워는 프로젝트의 스케일에 따라 조절하세요.
	DashPower = 5000.0f; 
}

void ASpeed_Bird::UseAbility()
{
	// 발사되었고, 충돌 전이며, 능력을 아직 쓰지 않았을 때
	if (BirdMesh && bHasLaunched && !bHasHitSomething && !bAbilityUsed)
	{
		// 1. 현재 날아가고 있는 방향 벡터 구하기
		FVector CurrentVelocity = BirdMesh->GetPhysicsLinearVelocity();
		FVector DashDirection = CurrentVelocity.GetSafeNormal();

		// 만약 정지 상태에서 쓴다면 전방 방향(GetActorForwardVector)을 대안으로 사용
		if (DashDirection.IsNearlyZero())
		{
			DashDirection = GetActorForwardVector();
		}

		// 2. 물리 법칙을 끄지 않고 "순간적인 힘(Impulse)"만 가합니다.
		// VelocityChange를 true로 하면 질량에 상관없이 일정한 속도 변화를 줍니다.
		BirdMesh->AddImpulse(DashDirection * DashPower, NAME_None, true);

		// 3. (선택 사항) 대시 느낌을 강조하기 위해 현재 속도를 대시 방향으로 정렬하고 싶다면:
		// BirdMesh->SetPhysicsLinearVelocity(DashDirection * DashPower, false);

		bAbilityUsed = true;
	}
}

void ASpeed_Bird::OnBirdHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// 부모의 로직 호출 (기본적인 충돌 처리 bHasHitSomething = true 등)
	Super::OnBirdHit(HitComponent, OtherActor, OtherComp, NormalImpulse, Hit);

	if (BirdMesh)
	{
		// 1. 충돌하는 순간, 현재 속도의 상당 부분을 깎아버립니다. (예: 70% 감소)
		FVector CurrentVelocity = BirdMesh->GetPhysicsLinearVelocity();
		BirdMesh->SetPhysicsLinearVelocity(CurrentVelocity * 0.3f);

		// 2. 바닥과의 마찰력을 높이기 위해 감쇠값을 일시적으로 매우 크게 줍니다.
		// 이 값이 높을수록 슬라이딩 없이 바닥에 '착' 달라붙거나 금방 멈춥니다.
		BirdMesh->SetLinearDamping(10.0f); 
		BirdMesh->SetAngularDamping(5.0f);
	}
}