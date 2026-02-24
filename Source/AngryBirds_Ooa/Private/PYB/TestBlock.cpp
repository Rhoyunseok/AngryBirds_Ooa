// Fill out your copyright notice in the Description page of Project Settings.


#include "PYB/TestBlock.h"
#include "Components/BoxComponent.h"


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
	// bIsGround = true;
}

// Called every frame
void ATestBlock::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 현재 상태 체크
	if (!bIsGround)
	{
		GroundHitResult = CheckGround();
		if (GroundHitResult.HitCount == 0) // 공중
		{
			AddGravity(DeltaTime);
		} 
		else if (GroundHitResult.HitCount < 4) // 불안정
		{
			SetTippingPhysics(GroundHitResult, DeltaTime);
		}
		else // 안정
		{
			SetActorGround();
		}
	}
	if (bIsMoving && bIsGround)
	{
		AddFriction(DeltaTime);
	}
	
	ApplyCurVelocity();
	UpdateActorLocation();
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
}

FCornerHitData ATestBlock::CheckGround()
{
	FVector BoxLoc = GetActorLocation();
	FQuat BoxQuat = GetActorQuat();
	FCollisionQueryParams params;
	FCornerHitData GroundResult;
	
	params.AddIgnoredActor(this);
	for (int i = 0; i < 8; i++)
	{
		FVector WorldCorner = BoxLoc + BoxQuat.RotateVector(LocalCorners[i]);
		FVector Start = WorldCorner + FVector(0, 0, 0.5f);
		FVector End = WorldCorner - FVector(0, 0, 0.5f);
	
		FHitResult hitInfo;
		if (GetWorld()->LineTraceSingleByChannel(hitInfo, Start, End, ECC_Visibility, params))
		{
			FVector PenetrationVec = WorldCorner - hitInfo.ImpactPoint;
			float DistanceAlongNormal = FVector::DotProduct(PenetrationVec, hitInfo.ImpactNormal);
			
			if (DistanceAlongNormal < 0.0f)
			{
				GroundResult.HitCount++;
				GroundResult.HitIndices.Add(i);
				GroundResult.AverageImpactPoint += hitInfo.ImpactPoint;
				float PenetrationDepth = FMath::Abs(DistanceAlongNormal);
				if (PenetrationDepth > 0.1f && PenetrationDepth > GroundResult.MaxPenetration)
				{
					GroundResult.MaxPenetration = PenetrationDepth;
					GroundResult.SurfaceNormal = hitInfo.ImpactNormal;
				}
			}
		}
	}
	
	if (GroundResult.HitCount > 0)
	{
		bIsGround = false;
		GroundResult.AverageImpactPoint /= GroundResult.HitCount;
	}
	
	return GroundResult;
}

void ATestBlock::SetActorGround()
{
	CurVelocity.Z = 0;
	FVector NewLocation = FVector(GetActorLocation().X, GetActorLocation().Y, BoxE.Z);
	SetActorLocation(NewLocation);
	bIsGround = true;
}

void ATestBlock::ApplyCurVelocity()
{	
	FVector VelXY = FVector(CurVelocity.X, CurVelocity.Y, 0.0f);
	VelXY = VelXY.GetClampedToMaxSize(MAX_MOVE_SPEED);
	float VelZ = CurVelocity.Z < MAX_FALL_SPEED ? CurVelocity.Z : MAX_FALL_SPEED;
	CurVelocity = VelXY;
	CurVelocity.Z = VelZ;
	//CurVelocity = CurVelocity.GetClampedToMaxSize(MAX_SPEED);

	bIsMoving = CurVelocity.Size2D() != 0;
}

void ATestBlock::UpdateActorLocation()
{
	AddActorWorldOffset(CurVelocity, false);
}

void ATestBlock::UpdateActorRotation(FCornerHitData& HitData, FQuat DeltaQuat)
{
	FVector PivotPoint = HitData.AverageImpactPoint;
	FVector DistanceToPivot = GetActorLocation() - PivotPoint;
	
	FQuat RotationThisFrame = DeltaQuat;
	FVector NewRelativeLocation = RotationThisFrame.RotateVector(DistanceToPivot);

	SetActorLocation(PivotPoint + NewRelativeLocation);
	AddActorWorldRotation(RotationThisFrame);
}

void ATestBlock::AddFriction(float DeltaSeconds)
{
	FVector FDirection = -CurVelocity.GetSafeNormal2D();
	CurVelocity.X += FDirection.X * MuMove * Mass * -Gravity;
	CurVelocity.Y += FDirection.Y * MuMove * Mass * -Gravity;
	if (FDirection.X < 0 == CurVelocity.X < 0) CurVelocity.X = 0;
	if (FDirection.Y < 0 == CurVelocity.Y < 0) CurVelocity.Y = 0;
}

void ATestBlock::SetTippingPhysics(FCornerHitData& HitData, float DeltaSeconds)
{
	CurVelocity.Z = 0;
	bIsGround = false;
	
	FVector Center = GetActorLocation();
	
	FVector ToImpact = HitData.AverageImpactPoint - Center;
	ToImpact.Normalize();
	
	FVector RotationAxis = FVector::CrossProduct(ToImpact, FVector(0, 0, -1));
	RotationAxis.Normalize();
	
	float TiltAmount = 1.0f - FVector::DotProduct(GetActorUpVector(), HitData.SurfaceNormal);
	TiltAmount = FMath::Clamp(TiltAmount, 0.0f, 1.0f);
	
	if (TiltAmount < 0.01f)
	{
		SetActorGround();
		return;
	}

	float RotationSpeed = 10.0f;
	float AngleToRotate = TiltAmount * RotationSpeed * DeltaSeconds;
	
	FQuat DeltaQuat = FQuat(RotationAxis, AngleToRotate * RotationSpeed * DeltaSeconds);
	GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Cyan, 
	FString::Printf(TEXT("%s"), *DeltaQuat.ToString()));
	
	UpdateActorRotation(HitData, DeltaQuat);
}
