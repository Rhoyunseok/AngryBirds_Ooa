#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
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

    UPROPERTY(VisibleAnywhere)
    class USceneComponent* RootScene;

    UPROPERTY(VisibleAnywhere)
    class USkeletalMeshComponent* BirdMesh;

    UPROPERTY(VisibleAnywhere)
    class USpringArmComponent* SpringArm;

    UPROPERTY(VisibleAnywhere)
    class UCameraComponent* FollowCamera;

    UPROPERTY(VisibleAnywhere)
    class UProjectileMovementComponent* ProjectileMovement;

    // --- 상태 변수 ---
    bool bHasLaunched = false;
    bool bHasHitSomething = false;
    bool bAbilityUsed = false;

    // --- 카메라 타겟 변수 ---
    UPROPERTY()
    AActor* ReturnTarget; // 새총에서 지정해줄 복귀용 고정 카메라/액터

    FTimerHandle DespawnTimerHandle;

    UFUNCTION()
    virtual void OnBirdHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

    void OnAbilityInput();
    virtual void UseAbility();
    void Launch(FVector LaunchVelocity);
    void LaunchByVector(FVector LaunchVelocity);

    // 카메라 복귀 시퀀스 함수
    void StartCameraReturn();
    void DestroyBird();
    
protected:
    float LaunchTime = 0.0f; // 발사 시점을 저장할 변수
    
};