// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TestBlock.generated.h"

USTRUCT()
struct FCornerHitData
{
	GENERATED_BODY()
	
	int HitCount = 0;
	UPROPERTY()
	TArray<int> HitIndices;
	float MaxPenetration = 0.0f;
	FVector SurfaceNormal = FVector::UpVector;
	FVector AverageImpactPoint = FVector::ZeroVector;
};

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
	FVector BoxE;
	FVector LocalCorners[8];
	
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
	UPROPERTY(EditAnywhere, Category="Movement") FCornerHitData GroundHitResult;
	
	
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
	void SetTippingPhysics(FCornerHitData& HitData, float DeltaSeconds);
	void SetActorGround();
	void ApplyCurVelocity();
	void UpdateActorLocation();
	void UpdateActorRotation(FCornerHitData& HitData, FQuat DeltaQuat);
	
	// 상태 관련 함수
	FCornerHitData CheckGround();
	// bool CheckMoving();
};
