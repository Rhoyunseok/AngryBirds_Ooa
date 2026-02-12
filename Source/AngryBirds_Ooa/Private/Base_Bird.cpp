#include "Base_Bird.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"

// --- 카메라와 스프링암을 위한 헤더 ---
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"


ABase_Bird::ABase_Bird()
{
	
    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;
    
    PrimaryActorTick.bCanEverTick = false;

	// 1. Root를 빈 컴포넌트로 설정 (이래야 메쉬를 마음대로 돌림)
	USceneComponent* SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	RootComponent = SceneRoot;

	// 2. 메쉬 설정 및 Root에 부착
	BirdMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BirdMesh"));
	BirdMesh->SetupAttachment(RootComponent);
    
	// 만약 메쉬가 옆을 보고 있다면 여기서 보정 (예: Yaw 90도)
	BirdMesh->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
    // --- 추가 시작: 카메라 시스템 생성 및 설정 ---
    
    // 2. 스프링 암(카메라 지지대) 생성 및 부착
    SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    SpringArm->SetupAttachment(RootComponent); // 메쉬에 매달기
    
    // 카메라 위치 세팅 (대각선 뒤쪽 상단)
    SpringArm->TargetArmLength = 400.0f;           // 카메라와 새 사이의 거리
    SpringArm->SetRelativeRotation(FRotator(-30.0f, 0.0f, 0.0f)); // Y축(Pitch) 회전: 위에서 아래로 내려다봄
    SpringArm->bDoCollisionTest = true;           // 장애물이 카메라 가리면 앞으로 당기기
    
    // 새가 날아갈 때 카메라가 같이 회전하지 않도록 고정
    SpringArm->bInheritPitch = false;
    SpringArm->bInheritYaw = false;
    SpringArm->bInheritRoll = false;

    // 3. 실제 카메라 생성 및 스프링 암 끝에 부착
    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);

    // 4. 발사체 컴포넌트 설정 (기존 코드)
    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
    ProjectileMovement->UpdatedComponent = RootComponent;
    ProjectileMovement->bAutoActivate = false; 
    ProjectileMovement->InitialSpeed = 0.f;
    ProjectileMovement->MaxSpeed = 5000.f; 
    ProjectileMovement->ProjectileGravityScale = 1.0f; 
}

// Launch 함수는 그대로 두시면 됩니다.
void ABase_Bird::Launch(float AngleDegrees, float Power)
{
    if (ProjectileMovement)
    {
       float Radians = FMath::DegreesToRadians(AngleDegrees);
       FVector LaunchDirection = FVector(FMath::Cos(Radians), 0.0f, FMath::Sin(Radians));

       ProjectileMovement->Velocity = LaunchDirection * Power;
       ProjectileMovement->Activate();

       UE_LOG(LogTemp, Warning, TEXT("Base_Bird 발사 완료! 각도: %f"), AngleDegrees);
    }
}