// Fill out your copyright notice in the Description page of Project Settings.


#include "PYB/BaseBlock.h"

#include "Base_Bird.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
ABaseBlock::ABaseBlock()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	bodyMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyMeshComponent"));
	RootComponent = bodyMeshComp;
	bodyMeshComp->SetCollisionProfileName(TEXT("BlockAll"));
	bodyMeshComp->SetNotifyRigidBodyCollision(true);
	bodyMeshComp->SetSimulatePhysics(true);
	bodyMeshComp->SetLinearDamping(1.0f);
	bodyMeshComp->SetAngularDamping(1.0f);
	
	bodyMeshComp->GetBodyInstance()->bOverrideMass = true;
	bodyMeshComp->GetBodyInstance()->SetMassOverride(100.0f);
}

// Called when the game starts or when spawned
void ABaseBlock::BeginPlay()
{
	Super::BeginPlay();
	
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ABaseBlock::EnableHitDamage, 0.2f, false);
	
	UMaterialInterface* BaseMaterial = bodyMeshComp->GetMaterial(0);
	if (BaseMaterial)
	{
		DynamicMaterial = bodyMeshComp->CreateDynamicMaterialInstance(0, BaseMaterial);
		bodyMeshComp->SetMaterial(0, DynamicMaterial);
		// UE_LOG(LogTemp, Warning, TEXT("Successfully Created: %s"), *DynamicMaterial->GetName());
	}
}

void ABaseBlock::EnableHitDamage()
{
	bodyMeshComp->OnComponentHit.AddDynamic(this, &ABaseBlock::OnBlockHit);
}

// Called every frame
void ABaseBlock::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

float ABaseBlock::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	// 블록의 HP 깎기
	BlockHP -= ActualDamage;
	UE_LOG(LogTemp, Warning, TEXT("BlockHP: %f, ActualDamage: %f"), BlockHP, ActualDamage);

	if (BlockHP > 0.0f)
	{
		UE_LOG(LogTemp, Warning, TEXT("다침"));
		if (DynamicMaterial)
		{
			DynamicMaterial->SetScalarParameterValue(FName("CrackAmount"), 0.8);
			// float temp = 0.0f;
			// DynamicMaterial->GetScalarParameterValue(FName("CrackAmount"), temp);
			// UE_LOG(LogTemp, Warning, TEXT("BlockHP: %f, DynamicMat: %s"), temp, *DynamicMaterial->GetName());
		}
		DamageState = 1;
	}
	else
	{
		DamageState = 2;
		BeforeBlockDestory();
		this->Destroy();
	}
	
	return ActualDamage;
}

void ABaseBlock::OnBlockHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!OtherActor) return;
	
	FVector HitNormal = Hit.ImpactNormal;
	FVector UpVector = FVector(0.f, 0.f, 1.0f);
	float AbsDot = FMath::Abs(FVector::DotProduct(HitNormal, UpVector));
	if (AbsDot > 0.95f) return;
	
	float SelectedThreshold = BlockThreshold;
	float OtherSpeed = OtherActor->GetVelocity().Size();
	
	if (HitComponent->IsSimulatingPhysics())
	{
		ABase_Bird* Bird = Cast<ABase_Bird>(OtherActor);
		if (Bird)
		{
			OtherSpeed= Bird->CustomVelocity.Size();
			UE_LOG(LogTemp, Log, TEXT("새 속도: %f"), OtherSpeed);
		}
	}
	
	if (OtherActor->IsA(ABase_Bird::StaticClass()))
	{
		SelectedThreshold = BirdThreshold;
		UE_LOG(LogTemp, Log, TEXT("새..."));
	}
	
	float ImpactSpeed = FMath::Abs(OtherSpeed - this->GetVelocity().Size());
	
	// UE_LOG(LogTemp, Warning, TEXT("ImpulseSize: %f, SelectedThreshold: %f"), ImpactSpeed, SelectedThreshold);
	
	if (ImpactSpeed > SelectedThreshold)
	{
		float CalculatedDamage = 50 + (ImpactSpeed - SelectedThreshold) / 10;

		// 나 자신에게 데미지를 입힘 (내가 부딪혀서 아픔)
		UGameplayStatics::ApplyDamage(this, CalculatedDamage, nullptr, OtherActor, UDamageType::StaticClass());
	}
}

void ABaseBlock::BeforeBlockDestory()
{
	OnScoreChanged.Broadcast(BlockPrice);
	UE_LOG(LogTemp, Warning, TEXT("파괴 %s"), *GetName());
}