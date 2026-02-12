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
	UPROPERTY(VisibleAnywhere, Category=Scene)
	class USceneComponent* DefaultSceneComp;
	UPROPERTY(EditAnywhere, Category=BodyMesh)
	class UStaticMeshComponent* bodyMeshComp;
	
	// 컴포넌트 관련 변수 선언
	float BoxZ;
	float CheckZ;
	
	// 물리 변수 선언
	UPROPERTY(EditAnywhere, Category="Physics") float InAngle;
	UPROPERTY(EditAnywhere, Category="Physics") float Mass;
	UPROPERTY(EditAnywhere, Category="Physics") float MuStop;
	UPROPERTY(EditAnywhere, Category="Physics") float MuMove;
	UPROPERTY(EditAnywhere, Category="Physics") float Gravity;
	UPROPERTY(EditAnywhere, Category="Physics") FVector CurVelocity;
	
	// 상태 변수 선언
	UPROPERTY(EditAnywhere, Category="Physics") bool bIsGround;
	// bool bUseGravity;
	
	// 실제 작용하는 힘 선언
	FVector FTotal;
	FVector FImpact;
	FVector Velocity;

	// 초기 상태 설정 함수
	void InitConstant();
	
	// 움직임 관련 함수
	void AddGravity(float DeltaSeconds);
	UFUNCTION(BlueprintCallable, Category="Physics")
	void AddForceVector(FVector InputForce);
	
	// 상태 관련 함수
	bool CheckGround();
};
