#pragma once



#include "CoreMinimal.h"

#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"

#include "Base_Bird.generated.h"



UCLASS()

class ANGRYBIRDS_OOA_API ABase_Bird : public APawn

{

    GENERATED_BODY()



public:

    ABase_Bird();



    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")

    class USceneComponent* RootScene;



    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")

    class USkeletalMeshComponent* BirdMesh;



    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")

    class UProjectileMovementComponent* ProjectileMovement;



    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")

    class USpringArmComponent* SpringArm;



    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")

    class UCameraComponent* FollowCamera;



    UFUNCTION(BlueprintCallable, Category = "Actions")

    void Launch(float VerticalAngle, float HorizontalAngle, float Power);



    UFUNCTION(BlueprintCallable, Category = "Actions")

    void LaunchByVector(FVector LaunchVelocity);


    virtual void Tick(float DeltaTime) override;

    void OnDragStart();

    virtual void OnDragRelease();
    
    FVector2D StartMousePos;

    FVector InitialLocation;

    bool bIsDragging = false;
    bool bIsSettling = false; // 서서히 배를 바닥에 대고 멈추는 상태인가?
    
    int32 HitCount = 0;
    const int32 MaxHitBeforeStop = 2; // 2번 충돌 후 멈춤
    
    int32 BounceCount = 0;
    bool bIsRolling = false;
    
    // 마우스 클릭 입력을 받았을 때 실행될 함수
    void OnAbilityInput();
    
protected:

    virtual void BeginPlay() override;

    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    UPROPERTY(EditAnywhere, Category = "Launch")

    float MaxDragDist = 500.0f;
    

    UPROPERTY(EditAnywhere, Category = "Drag")

    float MaxVisualDragDist = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status")

    bool bHasLaunched = false;

    UFUNCTION()
    virtual void OnBirdHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

    bool bHasHitSomething = false; // 충돌 여부 체크 변수
    
    void DestroyBird();

    // 충돌 후 소멸까지 기다릴 시간 (초)
    UPROPERTY(EditAnywhere, Category = "BirdSettings")
    float DespawnDelay = 3.0f;

    FTimerHandle DespawnTimerHandle;
    
    // 궤적을 그릴지 여부
    bool bShowTrajectory = false;

    // 궤적 계산 및 그리기 함수
    virtual void DisplayTrajectory();

    // MaxDragDist 등 기존 변수들 위치 확인
    
    // 능력을 이미 사용했는지 체크 (중복 실행 방지)
    bool bAbilityUsed = false;

    // 자식 클래스들이 각자의 능력을 구현할 '가상 함수'
    // virtual: 자식에서 이 함수를 재정의(Override)할 수 있게 허용함
    virtual void UseAbility();
};