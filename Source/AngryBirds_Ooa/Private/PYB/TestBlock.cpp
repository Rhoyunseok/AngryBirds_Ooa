// Fill out your copyright notice in the Description page of Project Settings.



#include "Components/BoxComponent.h"
#include "PYB/TestBlock.h"

#define MAX_MOVE_SPEED 500
#define MAX_FALL_SPEED 100

// Sets default values
ATestBlock::ATestBlock()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	// 콜리전
	CollisionComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollisionComp"));
	CollisionComp->SetCollisionProfileName(TEXT("BlockAll"));
	RootComponent = CollisionComp;
	
	// 메쉬
	BodyMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyMeshComp"));
	BodyMeshComp->SetupAttachment(CollisionComp);
	BodyMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//BodyMeshComp->SetRelativeScale3D(FVector(0.25f));
}

// Called when the game starts or when spawned
void ATestBlock::BeginPlay()
{
	Super::BeginPlay();
	
	InitConstant();
	
	
	// CurVelocity = FVector(50, 50, 0);
	// bIsMoving = true;
}

// Called every frame
void ATestBlock::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 현재 상태 체크
	// bIsGround = CheckGround().bBlockingHit;
	// bIsMoving = CheckMoving();
	if (!bIsGround)
	{
		GroundHitResult = CheckGround();
		// if (!GroundHitResult.bBlockingHit)
		// {
		// 	AddGravity(DeltaTime);
		// } else
		// {
		// 	SetActorGround();
		// }
	}
	if (bIsMoving && bIsGround)
	{
		AddFriction(DeltaTime);
	}
	
	ApplyCurVelocity();
	UpdateActorLocation();
	
	// 현재 속도 변경
	// if (bIsGround) AddGravity(DeltaTime);
	// if (bIsMoving) AddFriction(DeltaTime);
	
	//CurVelocity.X = FMath::FInterpTo(CurVelocity.X, 0.0f, DeltaTime, 2.0f);
	//CurVelocity.X = FMath::FInterpTo(CurVelocity.Y, 0.0f, DeltaTime, 2.0f);
	
	// FVector NewLocation = GetActorLocation() + (CurVelocity * DeltaTime);
	// SetActorLocation(NewLocation, true);
}

void ATestBlock::InitConstant()
{
	//InAngle = 0.0f;
	Mass = 1.f;
	// MuStop = 0.9f;
	MuMove = 0.5f;
	Gravity = -9.8f;
	CurVelocity = FVector::ZeroVector;
	
	BoxE = CollisionComp->Bounds.BoxExtent;
	LocalCorners[0] = FVector( BoxE.X,  BoxE.Y,  BoxE.Z);
	LocalCorners[1] = FVector( BoxE.X, -BoxE.Y,  BoxE.Z);
	LocalCorners[2] = FVector(-BoxE.X,  BoxE.Y,  BoxE.Z);
	LocalCorners[3] = FVector(-BoxE.X, -BoxE.Y,  BoxE.Z);
	LocalCorners[4] = FVector( BoxE.X,  BoxE.Y, -BoxE.Z);
	LocalCorners[5] = FVector( BoxE.X, -BoxE.Y, -BoxE.Z);
	LocalCorners[6] = FVector(-BoxE.X,  BoxE.Y, -BoxE.Z);
	LocalCorners[7] = FVector(-BoxE.X, -BoxE.Y, -BoxE.Z);
	
	bIsGround = false;
	bIsMoving = false;
}

void ATestBlock::AddGravity(float DeltaSeconds)
{
	CurVelocity.Z += Gravity * DeltaSeconds;
	
	
	
	
	// CurVelocity += Gravity * dt;
	// FVector NewLocation = GetActorLocation() + FVector(0, 0, VerticalVelocity * dt);
	// SetActorLocation(NewLocation, true);
}

// void ATestBlock::AddFriction(float DeltaSeconds)
// {
// 	if (CurVelocity.X > 0.1)
// 	{
// 		CurVelocity.X -= MuStop * DeltaSeconds;
// 	}
// 	if (CurVelocity.Y > 0.1)
// 	{
// 		CurVelocity.Y -= MuStop * DeltaSeconds;
// 	}
// }

FCornerHitData ATestBlock::CheckGround()
{
	//FHitResult hitInfo;
	FCollisionQueryParams params;
	//FVector startPos = CollisionComp->GetComponentLocation();
	//FVector endPos = startPos - 5.f;
	
	FVector BoxLoc = GetActorLocation();
	FQuat BoxQuat = GetActorQuat();
	
	params.AddIgnoredActor(this);
	//GetWorld()->LineTraceSingleByChannel(hitInfo, startPos, endPos, ECC_Visibility, params);
	FCornerHitData GroundResult;
	
	for (int i = 0; i < 8; i++)
	{
		FVector WorldCorner = BoxLoc + BoxQuat.RotateVector(LocalCorners[i]);
		FVector Start = WorldCorner + FVector(0, 0, 2.f);
		FVector End = WorldCorner - FVector(0, 0, 5.f);
	
		FHitResult hitInfo;
		if (GetWorld()->LineTraceSingleByChannel(hitInfo, Start, End, ECC_Visibility, params))
		{
			FVector PenetrationVec = WorldCorner - hitInfo.ImpactPoint;
			float DistanceAlongNormal = FVector::DotProduct(PenetrationVec, hitInfo.ImpactNormal);
			
			if (DistanceAlongNormal < 0.f)
			{
				GroundResult.HitCount++;
				GroundResult.HitIndices.Add(i);
				GroundResult.AverageImpactPoint += hitInfo.ImpactPoint;
				float PenetrationDepth = FMath::Abs(DistanceAlongNormal);
				if (PenetrationDepth > GroundResult.MaxPenetration)
				{
					GroundResult.MaxPenetration = PenetrationDepth;
					GroundResult.SurfaceNormal = hitInfo.ImpactNormal;
				}
			}
		}
	}
	
	if (GroundResult.HitCount > 0)
	{
		GroundResult.AverageImpactPoint /= GroundResult.HitCount;
	}
	
	return GroundResult;
}

// bool ATestBlock::CheckMoving()
// {
// 	return CurVelocity.Size2D() == 0;
// }

//void ATestBlock::AddForceVector(FVector InputForce)
// {
// 	CurVelocity += InputForce;
// }

void ATestBlock::SetActorGround()
{
	CurVelocity.Z = 0;
	FVector NewLocation = FVector(GetActorLocation().X, GetActorLocation().Y, GroundHitResult.ImpactNormal.Z);
	SetActorLocation(NewLocation);
	bIsGround = true;
}

void ATestBlock::ApplyCurVelocity()
{	
	FVector VelXY = FVector(CurVelocity.X, CurVelocity.Y, 0.f);
	VelXY = VelXY.GetClampedToMaxSize(MAX_MOVE_SPEED);
	float VelZ = CurVelocity.Z < MAX_FALL_SPEED ? CurVelocity.Z : MAX_FALL_SPEED;
	CurVelocity = VelXY;
	CurVelocity.Z = VelZ;
	//CurVelocity = CurVelocity.GetClampedToMaxSize(MAX_SPEED);

	bIsMoving = !(CurVelocity.Size2D() == 0);
}

void ATestBlock::UpdateActorLocation()
{
	AddActorWorldOffset(CurVelocity, false);
}

void ATestBlock::AddFriction(float DeltaSeconds)
{
	FVector FDirection = -CurVelocity.GetSafeNormal2D();
	CurVelocity.X += FDirection.X * MuMove * Mass * -Gravity;
	CurVelocity.Y += FDirection.Y * MuMove * Mass * -Gravity;
	if (FDirection.X < 0 == CurVelocity.X < 0) CurVelocity.X = 0;
	if (FDirection.Y < 0 == CurVelocity.Y < 0) CurVelocity.Y = 0;
	// if (CurVelocity.Size2D() == 0) bIsMoving = false;
}
