#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Base_Bird.generated.h"

class USkeletalMeshComponent;
class USpringArmComponent;
class UCameraComponent;
class UProjectileMovementComponent;

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
    UPROPERTY(VisibleAnywhere)
    USceneComponent* RootScene;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    USkeletalMeshComponent* BirdMesh;

    UPROPERTY(VisibleAnywhere)
    USpringArmComponent* SpringArm;

    UPROPERTY(VisibleAnywhere)
    UCameraComponent* FollowCamera;

    UPROPERTY(VisibleAnywhere)
    UProjectileMovementComponent* ProjectileMovement;

    // --- 외부 호출용 함수 ---
    UFUNCTION(BlueprintCallable)
    void LaunchByVector(FVector LaunchVelocity);

    // --- 입력 및 능력 관련 ---
    // 1. 입력 바인딩용 함수 (함수 포인터용)
    void OnAbilityInput();

    // 2. 실제 구현부 (자식에서 override)
    virtual void UseAbility();

    // --- 충돌 및 소멸 ---
    UFUNCTION()
    virtual void OnBirdHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

    void DestroyBird();
    
    //날라가는 함수 launch
    void Launch(FVector LaunchVelocity);
    
    bool bAbilityUsed = false;

protected:
    bool bHasLaunched = false;
    bool bHasHitSomething = false;
    
    
    FTimerHandle DespawnTimerHandle;
};