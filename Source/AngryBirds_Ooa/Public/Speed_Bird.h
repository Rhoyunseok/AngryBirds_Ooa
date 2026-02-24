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
	// 부모의 가상 함수 오버라이드
	virtual void UseAbility() override;
	virtual void OnBirdHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;

	// 대쉬 속도 (발사 속도보다 높게 설정)
	UPROPERTY(EditAnywhere, Category = "Ability")
	float DashPower;
};