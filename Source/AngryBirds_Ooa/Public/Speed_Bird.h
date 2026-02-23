#pragma once

#include "CoreMinimal.h"
#include "Base_Bird.h"
#include "Speed_Bird.generated.h"

UCLASS()
class ANGRYBIRDS_OOA_API ASpeed_Bird : public ABase_Bird
{
	GENERATED_BODY()

public:
	ASpeed_Bird();

protected:
	// 능력을 대쉬로 구현
	virtual void UseAbility() override;
	
	// 대쉬 중 충돌 시 상태를 복구하기 위해 오버라이드
	virtual void OnBirdHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;

	// 대쉬 속도 (부모의 발사 속도 2500보다 훨씬 높게 설정)
	UPROPERTY(EditAnywhere, Category = "Ability")
	float DashPower = 30000.0f;
};