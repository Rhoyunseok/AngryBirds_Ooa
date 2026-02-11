// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Base_Bird.generated.h"

UCLASS()
class ANGRYBIRDS_OOA_API ABase_Bird : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABase_Bird();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// --- 부품(컴포넌트) 선언 ---

	// 물리 충돌을 담당하는 구체 (뿌리)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USphereComponent* SphereComp;

	// 새의 외관 (에셋을 입힐 곳)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UStaticMeshComponent* MeshComp;

	// 발사 후 날아가는 물리 로직 담당
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UProjectileMovementComponent* ProjectileMovement;

	// 카메라를 매달 '셀카봉' (스프링암)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USpringArmComponent* SpringArm;

	// 새를 따라다닐 카메라
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UCameraComponent* FollowCamera;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	// 나중에 새총에서 호출할 "발사!" 함수
	void Launch(FVector LaunchDirection, float Speed);

};
