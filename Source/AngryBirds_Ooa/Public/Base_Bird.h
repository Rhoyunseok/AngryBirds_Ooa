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



protected:

    virtual void BeginPlay() override;

    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;



public:

    virtual void Tick(float DeltaTime) override;

    void OnDragStart();

    void OnDragRelease();



protected:

    UPROPERTY(EditAnywhere, Category = "Launch")

    float MaxDragDist = 500.0f;



    UPROPERTY(EditAnywhere, Category = "Drag")

    float MaxVisualDragDist = 100.0f;



    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status")

    bool bHasLaunched = false;



private:

    FVector2D StartMousePos;

    FVector InitialLocation;

    bool bIsDragging = false;

};