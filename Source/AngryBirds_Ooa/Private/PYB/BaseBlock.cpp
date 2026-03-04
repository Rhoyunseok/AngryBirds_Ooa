// Fill out your copyright notice in the Description page of Project Settings.


#include "PYB/BaseBlock.h"

#include "Base_Bird.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ABaseBlock::ABaseBlock()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	SceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	RootComponent = SceneComp;
	
	bodyMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyMeshComponent"));
	bodyMeshComp->SetupAttachment(SceneComp);
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

	if (BlockHP > 0.0f)
	{
		if (DynamicMaterial)
		{
			DynamicMaterial->SetScalarParameterValue(FName("DamageAlpha"), 0.5);
		}
		DamageState = 1;
	}
	else
	{
		DamageState = 2;
		DestroyBlock();
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
	
	if (OtherActor->IsA(ABase_Bird::StaticClass()))
	{
		SelectedThreshold = BirdThreshold;
	}
	
	// 1. 충격량(Impulse)의 크기를 가져옵니다.
	// NormalImpulse는 물리 엔진이 계산한 '충돌 힘'입니다.
	float ImpulseSize = NormalImpulse.Size();
	
	// 2. 데미지로 인정할 최소 충격량 설정 (Threshold)
	// 이 값보다 작으면 그냥 바닥에 닿아 있거나 살짝 굴러가는 상태로 간주합니다.
	float BlockMass = bodyMeshComp->GetMass();
	float DynamicThreshold = (FMath::Loge(BlockMass + 1.0f) * SelectedThreshold) + DefaultThreshold;
	
	UE_LOG(LogTemp, Warning, TEXT("ImpulseSize: %f, DynamicThreshold: %f"), ImpulseSize, DynamicThreshold);
	
	if (ImpulseSize > DynamicThreshold)
	{
		float CalculatedDamage = (ImpulseSize - DynamicThreshold) / (BlockMass * 0.1f);

		// 나 자신에게 데미지를 입힘 (내가 부딪혀서 아픔)
		UGameplayStatics::ApplyDamage(this, CalculatedDamage, nullptr, OtherActor, UDamageType::StaticClass());
	}
}

void ABaseBlock::DestroyBlock()
{
	SetLifeSpan(0.01f);
}
