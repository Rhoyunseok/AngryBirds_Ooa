// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Base_Bird.h"
#include "GameFramework/Actor.h"
#include "BaseBlock.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnScoreChangedSignature, float);

UCLASS()
class ANGRYBIRDS_OOA_API ABaseBlock : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseBlock();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION()
	float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, 
		class AController* EventInstigator, AActor* DamageCauser) override;
	
	UFUNCTION()
	void OnBlockHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		FVector NormalImpulse, const FHitResult& Hit);
	
	UFUNCTION()
	void EnableHitDamage();

	UPROPERTY(VisibleAnywhere)
	class USceneComponent* SceneComp;
	UPROPERTY(VisibleAnywhere, Category=BodyMesh)
	class UStaticMeshComponent* bodyMeshComp;
	
	UPROPERTY(EditAnywhere, Category=BlockState)
	float BlockMaxHP = 100;
	UPROPERTY(EditAnywhere, Category=BlockState)
	float BlockHP = 100;
	UPROPERTY(EditAnywhere, Category=BlockState)
	float BlockPrice = 100;
	
	UPROPERTY(EditAnywhere, Category=BlockState)
	float BirdThreshold = 100.0f;
	UPROPERTY(EditAnywhere, Category=BlockState)
	float BlockThreshold = 300.0f;
	
	UPROPERTY()
	class UMaterialInstanceDynamic* DynamicMaterial;
	
	UPROPERTY(EditAnywhere)
	class AAngryBirdGameState* GameState;
	
	// 블록 데미지 단계: 0 - 정상, 1 - 깨짐, 2 - 파괴
	int DamageState = 0;
	float CalculatedDamage = 0.0f;
	float BaseDamage = 50.0f;
	ABase_Bird* Bird;
	
	virtual void CalBirdDamage();
	virtual void BeforeBlockDestory();
	
	UPROPERTY(EditAnywhere)
	USoundBase* HitSound;
	UPROPERTY(EditAnywhere)
	USoundBase* BreakSound;
	UPROPERTY(EditAnywhere)
	UParticleSystem* HitParticle;
	UPROPERTY(EditAnywhere)
	UParticleSystem* BreakParticle;
	
	FOnScoreChangedSignature OnScoreChanged;
};
