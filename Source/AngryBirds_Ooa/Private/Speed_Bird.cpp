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
	// 충돌 시에는 부모의 로직(소멸 타이머 등)을 실행해야 하므로 Super를 호출합니다.
	Super::OnBirdHit(HitComponent, OtherActor, OtherComp, NormalImpulse, Hit);

	if (BirdMesh)
	{
		// 충돌 후에는 다시 중력을 켜고 회전 잠금을 해제합니다.
		BirdMesh->SetEnableGravity(true);
		BirdMesh->SetLinearDamping(20.0f); 
		BirdMesh->BodyInstance.bLockXRotation = false;
		BirdMesh->BodyInstance.bLockYRotation = false;
		BirdMesh->BodyInstance.bLockZRotation = false;
		BirdMesh->BodyInstance.UpdatePhysicsFilterData();
	}
}