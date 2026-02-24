#pragma once

#include "CoreMinimal.h"
#include "Base_Bird.h"
#include "Engine/EngineTypes.h" 
#include "Bomb_Bird.generated.h"

UCLASS()
class ANGRYBIRDS_OOA_API ABomb_Bird : public ABase_Bird
{
	GENERATED_BODY()

public:
	ABomb_Bird();

	// 부모 클래스의 가상 함수 오버라이드
	virtual void UseAbility() override;

	// 부모 클래스의 충돌 함수 오버라이드
	virtual void OnBirdHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;

protected:
	// 폭발 반지름
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bomb Settings")
	float ExplosionRadius;

	// 폭발 위력
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bomb Settings")
	float ExplosionStrength;

	// 폭발을 실행하는 함수
	UFUNCTION() 
	void Explode();

private:
	// 지연 폭발을 위한 타이머 핸들
	FTimerHandle BombTimerHandle;
};