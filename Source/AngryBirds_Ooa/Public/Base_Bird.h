#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/SphereComponent.h"
#include "Sound/SoundBase.h"          // 사운드 추가
#include "Particles/ParticleSystem.h" // 이펙트 추가
#include "NiagaraFunctionLibrary.h"   // 나이아가라
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
public:
    virtual void DestroyBird();
    
protected:
    float LaunchTime = 0.0f; 
    
protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class USphereComponent* CollisionSphere;
    
protected:
    // --- 새 고유 사운드 에셋 (에디터에서 할당) ---
    UPROPERTY(EditAnywhere, Category = "Bird|Sound")
    USoundBase* ReadyVoiceSound; // 1. 장전 시 (기 모으는 기합: "으랴앗!")

    UPROPERTY(EditAnywhere, Category = "Bird|Sound")
    USoundBase* FlyingVoiceSound; // 2. 발사 시 (날아가는 소리: "포잉~~~")

    UPROPERTY(EditAnywhere, Category = "Bird|Sound")
    USoundBase* PainVoiceSound;   // 3. 충돌 시 (신음 소리: "퍽! 아야!")

    // --- 이펙트 에셋 (에디터에서 할당) ---
    UPROPERTY(EditAnywhere, Category = "Bird|Effects")
    UParticleSystem* HitParticle; // 충돌 시 파편 이펙트

    // 사운드 재생 공통 함수
    void PlayBirdSound(USoundBase* SoundToPlay);

public:
    // 새총에서 호출할 장전 사운드(기합) 재생용
    void PlayReadyVoice();
    
protected:
    // --- 이펙트 에셋 ---
    UPROPERTY(EditAnywhere, Category = "Bird|Effects")
    class UNiagaraSystem* FlyingTrailEffect; // 날아갈 때 뒤에 남는 연기 이펙트

    // 연기를 주기적으로 생성하기 위한 타이머
    FTimerHandle TrailTimerHandle;
    
    //새의 무게 변수: 에디터 디테일 패널 및 블루프린트에서 접근 가능
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bird|Stats")
    float BirdWeight = 10.0f;
    
public:    
    // 연기를 실제로 스폰하는 함수
    void SpawnTrail();
    
protected:
    // [추가] 능력 사용 시 재생할 사운드 (공통)
    UPROPERTY(EditAnywhere, Category = "Bird|Sound")
    USoundBase* AbilityVoiceSound; 
    
    // [추가] 능력 사용 시 재생할 나이아가라 이펙트 (공통)
    UPROPERTY(EditAnywhere, Category = "Bird|Effects")
    class UNiagaraSystem* AbilityNiagaraEffect;

    // [추가] 능력 사용 시 사운드와 이펙트를 동시에 처리하는 공통 함수
    void PlayAbilityEffects();
    
};