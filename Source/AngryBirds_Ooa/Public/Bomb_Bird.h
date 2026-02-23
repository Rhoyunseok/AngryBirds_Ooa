#pragma once

#include "CoreMinimal.h"
#include "Base_Bird.h"
// 타이머 기능을 위해 필요한 헤더를 미리 넣어두는 것이 안전합니다.
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
	UFUNCTION() // 타이머나 델리게이트에서 호출될 수 있으므로 UFUNCTION을 붙이는 게 안전합니다.
	void Explode();

private:
	// 지연 폭발을 위한 타이머 핸들
	FTimerHandle BombTimerHandle;
	
protected:
	// 부모의 궤적 표시 기능을 재정의합니다.
	virtual void DisplayTrajectory() override;
	
};