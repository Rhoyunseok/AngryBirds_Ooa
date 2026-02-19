#include "Speed_Bird.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"

ASpeed_Bird::ASpeed_Bird()
{
	// 부모의 계층 구조(RootScene -> BirdMesh -> SpringArm)를 건드리지 않습니다.
	DashPower = 3000.0f;
}

void ASpeed_Bird::UseAbility()
{
	// 부모의 bHasLaunched, bHasHitSomething 변수를 그대로 활용합니다.
	if (BirdMesh && bHasLaunched && !bHasHitSomething && !bAbilityUsed)
	{
		// 1. 대쉬 중에는 중력을 잠시 끄고 공기 저항을 없앱니다.
		BirdMesh->SetEnableGravity(false);
		BirdMesh->SetLinearDamping(0.0f);

		// 2. 방향: 현재 새가 날아가고 있는 방향(속도 벡터)을 추출합니다.
		FVector DashDirection = BirdMesh->GetPhysicsLinearVelocity().GetSafeNormal();
        
		// 3. 기존 속도를 무시하고 대쉬 파워로 즉시 덮어씌웁니다.
		BirdMesh->SetPhysicsLinearVelocity(DashDirection * DashPower, false);

		// 4. 대쉬 중에 새가 팽이처럼 돌지 않도록 회전을 잠급니다.
		BirdMesh->BodyInstance.bLockXRotation = true;
		BirdMesh->BodyInstance.bLockYRotation = true;
		BirdMesh->BodyInstance.bLockZRotation = true;
		BirdMesh->BodyInstance.UpdatePhysicsFilterData();

		bAbilityUsed = true;
	}
}

void ASpeed_Bird::OnBirdHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// 1. 부모의 로직 호출 (bHasHitSomething = true로 만들어 부모 Tick의 회전 간섭을 차단)
	Super::OnBirdHit(HitComponent, OtherActor, OtherComp, NormalImpulse, Hit);

	if (BirdMesh)
	{
		// [중요] 가속 시 껐던 물리 설정을 여기서 다시 다 킵니다.
        
		// 2. 중력 다시 활성화
		BirdMesh->SetEnableGravity(true);

		// 3. 공기 저항(Damping) 복구 (부모보다 좀 더 세게 줘서 팅겨나감을 방지)
		BirdMesh->SetLinearDamping(20.0f); 
		BirdMesh->SetAngularDamping(2.0f);

		// 4. 대쉬 때 잠갔던 회전축들 다 풀어주기 (이제 자유롭게 굴러야 함)
		BirdMesh->BodyInstance.bLockXRotation = false;
		BirdMesh->BodyInstance.bLockYRotation = false;
		BirdMesh->BodyInstance.bLockZRotation = false;
		BirdMesh->BodyInstance.UpdatePhysicsFilterData();

		// 5. 물리 엔진 강제 업데이트
		// 속도를 0으로 밀어버리고 물리 엔진을 한번 깨워줍니다.
		BirdMesh->SetPhysicsLinearVelocity(FVector::ZeroVector);
		BirdMesh->WakeAllRigidBodies();

		// 6. 바닥에 툭 떨어지는 느낌을 위해 아래 방향으로 살짝 밀어주기 (선택 사항)
		BirdMesh->AddImpulse(FVector(0, 0, -500.0f), NAME_None, true);
	}
}

