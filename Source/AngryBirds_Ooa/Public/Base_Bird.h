// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Base_Bird.generated.h"

UCLASS()
class ANGRYBIRDS_OOA_API ABase_Bird : public APawn
{
	GENERATED_BODY()
	
public:
	ABase_Bird();

	// 1. 컴포넌트 선언 (블루프린트에서 볼 수 있게 설정)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USkeletalMeshComponent* BirdMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UProjectileMovementComponent* ProjectileMovement;

	// 2. 발사 함수 (블루프린트에서 호출 가능하게 설정)
	UFUNCTION(BlueprintCallable, Category = "Actions")
	void Launch(float AngleDegrees, float Power);
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	class USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	class UCameraComponent* FollowCamera;
	
	// 메쉬의 기본 회전값을 블루프린트에서 수정할 수 있게 합니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	FRotator MeshDefaultRotation;
};
