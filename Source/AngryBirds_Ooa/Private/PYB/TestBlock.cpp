// Fill out your copyright notice in the Description page of Project Settings.


#include "PYB/TestBlock.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Materials/MaterialExpressionSubstrate.h"

// Sets default values
ATestBlock::ATestBlock()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	DefaultSceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneComp"));
	RootComponent = DefaultSceneComp;
	
	bodyMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyMeshComp"));
	bodyMeshComp->SetupAttachment(DefaultSceneComp);
	bodyMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//bodyMeshComp->SetRelativeScale3D(FVector(0.25f));
}

// Called when the game starts or when spawned
void ATestBlock::BeginPlay()
{
	Super::BeginPlay();
	
	InitConstant();
}

// Called every frame
void ATestBlock::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	AddGravity(DeltaTime);
	
	//CurVelocity.X = FMath::FInterpTo(CurVelocity.X, 0.0f, DeltaTime, 2.0f);
	//CurVelocity.X = FMath::FInterpTo(CurVelocity.Y, 0.0f, DeltaTime, 2.0f);
	
	FVector NewLocation = GetActorLocation() + (CurVelocity * DeltaTime);
	SetActorLocation(NewLocation, true);
}

void ATestBlock::InitConstant()
{
	InAngle = 0.0f;
	Mass = 100.0f;
	MuStop = 0.0f;
	MuMove = 0.0f;
	Gravity = -980.0f;
	CurVelocity = FVector::ZeroVector;
	
	CheckZ = 3.0f;
	BoxZ = bodyMeshComp->Bounds.BoxExtent.Z;
	
	bIsGround = CheckGround();
}

void ATestBlock::AddGravity(float dt)
{
	bIsGround = CheckGround();
	if (bIsGround)
	{
		CurVelocity.Z = 0;
		return;
	}
	
	CurVelocity.Z += Gravity * dt;
	
	// CurVelocity += Gravity * dt;
	// FVector NewLocation = GetActorLocation() + FVector(0, 0, VerticalVelocity * dt);
	// SetActorLocation(NewLocation, true);
}

bool ATestBlock::CheckGround()
{
	FHitResult hitInfo;
	FCollisionQueryParams params;
	FVector centerPos = bodyMeshComp->GetComponentLocation();
	FVector floorPos = centerPos - BoxZ - CheckZ;
		
	bool bHit = GetWorld()->LineTraceSingleByChannel(hitInfo, centerPos, floorPos, ECC_Visibility, params);
	return bHit;
}

void ATestBlock::AddForceVector(FVector InputForce)
{
	CurVelocity += InputForce;
}
