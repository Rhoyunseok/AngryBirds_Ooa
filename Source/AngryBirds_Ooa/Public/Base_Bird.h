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

    void OnDragRelease();
    
    FVector2D StartMousePos;

    FVector InitialLocation;

    bool bIsDragging = false;
    bool bIsSettling = false; // 서서히 배를 바닥에 대고 멈추는 상태인가?
    
    int32 HitCount = 0;
    const int32 MaxHitBeforeStop = 2; // 2번 충돌 후 멈춤
    
    int32 BounceCount = 0;
    bool bIsRolling = false;
    
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
    void OnBirdHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

    bool bHasHitSomething = false; // 충돌 여부 체크 변수
    
    void DestroyBird();

    // 충돌 후 소멸까지 기다릴 시간 (초)
    UPROPERTY(EditAnywhere, Category = "BirdSettings")
    float DespawnDelay = 3.0f;

    FTimerHandle DespawnTimerHandle;
    
    // 궤적을 그릴지 여부
    bool bShowTrajectory = false;

    // 궤적 계산 및 그리기 함수
    void DisplayTrajectory();

    // MaxDragDist 등 기존 변수들 위치 확인
};