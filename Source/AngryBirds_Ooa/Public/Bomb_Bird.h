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
    
	// 폭발 파티클 (Cascade)
	UPROPERTY(EditAnywhere, Category = "Bird|Effects")
	class UParticleSystem* ExplosionParticle;

	virtual void Tick(float DeltaTime) override;
	virtual void UseAbility() override;
	virtual void OnBirdHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bomb Settings")
	float ExplosionRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bomb Settings")
	float ExplosionStrength;

	// 지연 시간 (1.5 ~ 2.0초 권장)
	UPROPERTY(EditAnywhere, Category = "Bomb Settings")
	float FuseDuration = 1.75f;

	// 카메라 흔들림
	UPROPERTY(EditAnywhere, Category = "Effects")
	TSubclassOf<class UCameraShakeBase> ExplosionCameraShake;

	UFUNCTION() 
	void Explode();

	// [추가] 공통 지연 로직 함수
	void StartFuseSequence();

private:
	FTimerHandle BombTimerHandle;
    
	UPROPERTY()
	class UMaterialInstanceDynamic* DynamicMat;

	bool bIsIgnited = false;
	float CurrentFuseTime = 0.0f;
};