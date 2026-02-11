// Fill out your copyright notice in the Description page of Project Settings.


#include "Base_Bird.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

// Sets default values
ABase_Bird::ABase_Bird()
{
	// 매 프레임 업데이트(Tick)가 필요 없으므로 꺼서 최적화합니다.
	PrimaryActorTick.bCanEverTick = false;

	// 1. 충돌 구체 생성 및 루트(중심)로 설정
	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	RootComponent = SphereComp;
	SphereComp->SetSphereRadius(20.0f);
	SphereComp->SetSimulatePhysics(false); // 처음엔 새총에 고정되어야 하니 물리 꺼둠

	// 2. 메쉬(외관) 생성 및 충돌체에 부착
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(RootComponent);

	// 3. 발사체 컴포넌트 설정
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->UpdatedComponent = SphereComp;
	ProjectileMovement->InitialSpeed = 0.f;
	ProjectileMovement->MaxSpeed = 5000.f;
	ProjectileMovement->bAutoActivate = false; // 발사 전엔 작동 안함
	ProjectileMovement->ProjectileGravityScale = 1.0f; // 중력 영향 받음

	// 4. 스프링암(셀카봉) 설정
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 400.0f; // 카메라와의 거리
	SpringArm->bEnableCameraLag = true; // 카메라가 부드럽게 따라오게 함
	SpringArm->CameraLagSpeed = 5.0f;

	// 5. 카메라 생성 및 스프링암에 부착
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(SpringArm);

}

// Called when the game starts or when spawned
void ABase_Bird::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABase_Bird::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

/*void ABase_bird::Launch(FVector LaunchDirection, float Speed)
{
	// 이 함수가 호출되면 물리와 발사 로직이 켜집니다.
	SphereComp->SetSimulatePhysics(true);
	ProjectileMovement->Velocity = LaunchDirection * Speed;
	ProjectileMovement->Activate();
}*/

