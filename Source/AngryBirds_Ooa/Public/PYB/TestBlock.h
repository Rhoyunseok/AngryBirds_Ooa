// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TestBlock.generated.h"

UCLASS()
class ANGRYBIRDS_OOA_API ATestBlock : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATestBlock();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
public:
	// 컴포넌트 설정
	UPROPERTY(VisibleAnywhere, Category=Collision)
	class UBoxComponent* CollisionComp;
	UPROPERTY(EditAnywhere, Category=BodyMesh)
	class UStaticMeshComponent* BodyMeshComp;
	
	// 컴포넌트 관련 변수 선언
	float BoxZ;
	
	// 물리 변수 선언
	//UPROPERTY(EditAnywhere, Category="Physics") float InAngle;
	UPROPERTY(EditAnywhere, Category="Physics") float Mass;
	// UPROPERTY(EditAnywhere, Category="Physics") float MuStop;
	UPROPERTY(EditAnywhere, Category="Physics") float MuMove;
	UPROPERTY(EditAnywhere, Category="Physics") float Gravity;
	
	// 상태 변수 선언
	UPROPERTY(EditAnywhere, Category="State") bool bIsGround;
	UPROPERTY(EditAnywhere, Category="State") bool bIsMoving;
	// bool bUseGravity;
	
	// 운동 관련 변수 선언
	UPROPERTY(EditAnywhere, Category="Movement") FVector CurVelocity;
	UPROPERTY(EditAnywhere, Category="Movement") FHitResult GroundHitResult;
	
	
	// 실제 작용하는 힘 선언
	// FVector FTotal;
	// FVector FImpact;
	// FVector Velocity;

	// 초기 상태 설정 함수
	virtual void InitConstant();
	
	// 움직임 관련 함수
	void AddGravity(float DeltaSeconds);
	void AddFriction(float DeltaSeconds);
	// UFUNCTION(BlueprintCallable, Category="Movement")
	// void AddForceVector(FVector InputForce);
	void SetActorGround();
	void ApplyCurVelocity();
	void UpdateActorLocation();
	
	// 상태 관련 함수
	FHitResult CheckGround();
	// bool CheckMoving();
};
