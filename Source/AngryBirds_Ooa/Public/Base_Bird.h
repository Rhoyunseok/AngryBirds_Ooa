#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/SphereComponent.h"
#include "Base_Bird.generated.h"

UCLASS()
class ANGRYBIRDS_OOA_API ABase_Bird : public APawn
{
    GENERATED_BODY()

public:
    ABase_Bird();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    // --- 컴포넌트 ---
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class USceneComponent* RootScene;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class USkeletalMeshComponent* BirdMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class USpringArmComponent* SpringArm;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class UCameraComponent* FollowCamera;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class UProjectileMovementComponent* ProjectileMovement;

    // --- 상태 변수 ---
    bool bHasLaunched = false;
    bool bHasHitSomething = false;
    bool bAbilityUsed = false;

    // --- 커스텀 물리 시스템 변수 ---
    bool bUseCustomPhysics = false;     // 커스텀 물리 활성화 플래그
    FVector CustomVelocity;            // 현재 속도 벡터
    FVector GravityConstant;           // 중력 가속도

    // --- 카메라 타겟 변수 ---
    UPROPERTY()
    AActor* ReturnTarget; 

    FTimerHandle DespawnTimerHandle;

    // --- 주요 함수 ---
    UFUNCTION()
    virtual void OnBirdHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

    void OnAbilityInput();
    virtual void UseAbility();
    void Launch(FVector LaunchVelocity);
    void LaunchByVector(FVector LaunchVelocity);

    // 커스텀 물리 핵심 처리 함수
    void HandleCustomPhysics(float DeltaTime);

    // 카메라 복귀 시퀀스 함수
    void StartCameraReturn();
    void DestroyBird();
    
protected:
    float LaunchTime = 0.0f; 
    
protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class USphereComponent* CollisionSphere;
};