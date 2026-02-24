#include "Speed_Bird.h"
#include "Components/SkeletalMeshComponent.h"

ASpeed_Bird::ASpeed_Bird()
{
	// 프로젝트 스케일에 맞춰 기본값 설정
	DashPower = 5000.0f; 
}

void ASpeed_Bird::UseAbility()
{
	// 부모 클래스의 OnAbilityInput에서 이미 bAbilityUsed, bHasLaunched 등의 
	// 기본 조건 검사를 마친 후 실행되므로, 여기서는 핵심 로직에만 집중합니다.
	if (BirdMesh)
	{
		// 1. 현재 날아가고 있는 방향 벡터 구하기
		FVector CurrentVelocity = BirdMesh->GetPhysicsLinearVelocity();
		FVector DashDirection = CurrentVelocity.GetSafeNormal();

		// 비정상적인 상황(정지 등) 대비
		if (DashDirection.IsNearlyZero())
		{
			DashDirection = GetActorForwardVector();
		}

		// 2. 대시 적용: VelocityChange를 true로 하여 질량 무관 즉각적인 속도 변화 부여
		// Impulse 대신 SetPhysicsLinearVelocity를 써서 속도를 "고정"하는 방식이 
		// 게임적으로는 더 명확한 대시 느낌을 줍니다.
		BirdMesh->SetPhysicsLinearVelocity(DashDirection * DashPower, false);

		UE_LOG(LogTemp, Log, TEXT("Speed Bird: Dash Ability Used!"));
	}
}

void ASpeed_Bird::OnBirdHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// 부모의 기본 충돌 로직(타이머 시작, 상태 변경 등) 먼저 실행
	Super::OnBirdHit(HitComponent, OtherActor, OtherComp, NormalImpulse, Hit);

	if (BirdMesh)
	{
		// 스피드버드는 빠르기 때문에 충돌 시 더 강력한 마찰 저항을 줍니다.
		// 부모 클래스(10.0f)보다 높은 값을 주어 더 빠르게 멈추게 할 수 있습니다.
		BirdMesh->SetLinearDamping(15.0f); 
		BirdMesh->SetAngularDamping(8.0f);
       
		// 충돌 후 튕겨나가는 속도 제어 (잔여 속도의 20%만 유지)
		FVector SlowVelocity = BirdMesh->GetPhysicsLinearVelocity() * 0.2f;
		BirdMesh->SetPhysicsLinearVelocity(SlowVelocity);
	}
}