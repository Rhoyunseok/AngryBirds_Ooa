// Fill out your copyright notice in the Description page of Project Settings.


#include "PYB/BaseBlock.h"

#include "Base_Bird.h"
#include "Kismet/GameplayStatics.h"
#include "RHO/AngryBirdGameState.h"


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
	bodyMeshComp->SetLinearDamping(0.1f);
	bodyMeshComp->SetAngularDamping(0.5f);
	bodyMeshComp->BodyInstance.SleepFamily = ESleepFamily::Sensitive;
	
	bodyMeshComp->GetBodyInstance()->bOverrideMass = true;
	bodyMeshComp->GetBodyInstance()->SetMassOverride(100.0f);
	
	ConstructorHelpers::FObjectFinder<USoundBase> tempHitSound(TEXT("/Script/Engine.SoundWave'/Game/PYB/Sounds/wood_hit.wood_hit'"));
	if (tempHitSound.Succeeded())
	{
		HitSound = tempHitSound.Object;
	}
	ConstructorHelpers::FObjectFinder<USoundBase> tempBreakSound(TEXT("/Script/Engine.SoundWave'/Game/PYB/Sounds/wood_broke.wood_broke'"));
	if (tempBreakSound.Succeeded())
	{
		BreakSound = tempBreakSound.Object;
	}
	ConstructorHelpers::FObjectFinder<UParticleSystem> tempHitParticle(TEXT("/Script/Engine.ParticleSystem'/Game/Realistic_Starter_VFX_Pack_Vol2/Particles/Hit/P_Wood.P_Wood'"));
	if (tempHitParticle.Succeeded())
	{
		HitParticle = tempHitParticle.Object;
	}
	ConstructorHelpers::FObjectFinder<UParticleSystem> tempBreakParticle(TEXT("/Script/Engine.ParticleSystem'/Game/Realistic_Starter_VFX_Pack_Vol2/Particles/Hit/P_Concrete.P_Concrete'"));
	if (tempBreakParticle.Succeeded())
	{
		BreakParticle = tempBreakParticle.Object;
	}
}

// Called when the game starts or when spawned
void ABaseBlock::BeginPlay()
{
	Super::BeginPlay();
	
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ABaseBlock::EnableHitDamage, 5.0f, false);
	
	GameState = Cast<AAngryBirdGameState>(UGameplayStatics::GetGameState(this));
	
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
	
	if (GetVelocity().Size() > 0)
	{
		FVector GravityForce = FVector(0.f, 0.f, -2000.f);
		bodyMeshComp->AddForce(GravityForce, NAME_None, true);
	}
}

float ABaseBlock::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	// 블록의 HP 깎기
	BlockHP -= ActualDamage;
	// UE_LOG(LogTemp, Warning, TEXT("BlockHP: %f, ActualDamage: %f"), BlockHP, ActualDamage);

	if (BlockHP > 0.0f)
	{
		// UE_LOG(LogTemp, Warning, TEXT("다침"));
		if (DynamicMaterial)
		{
			DynamicMaterial->SetScalarParameterValue(FName("CrackAmount"), 0.8);
			// float temp = 0.0f;
			// DynamicMaterial->GetScalarParameterValue(FName("CrackAmount"), temp);
			// UE_LOG(LogTemp, Warning, TEXT("BlockHP: %f, DynamicMat: %s"), temp, *DynamicMaterial->GetName());
		}
		if (DamageState == 0)
		{
			DamageState = 1;
			if (HitSound)
			{
				UGameplayStatics::PlaySoundAtLocation(
					GetWorld(),
					HitSound,
					GetActorLocation(),
					1.0f,
					1.0f,
					0.0f,
					nullptr,
					nullptr
					);
			}
			if (HitParticle && GetWorld())
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitParticle, GetActorLocation());
			}
		}
	}
	else if (DamageState != 2)
	{
		DamageState = 2;
		if (BreakSound)
		{
			UGameplayStatics::PlaySoundAtLocation(
				GetWorld(),
				BreakSound,
				GetActorLocation(),
				0.8f,
				1.0f,
				0.0f,
				nullptr,
				nullptr
				);
		}
		if (BreakParticle && GetWorld())
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BreakParticle, GetActorLocation());
		}
		BeforeBlockDestory();
		FTimerHandle TimerHandle;
		GetWorldTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([this]()
		{
			// 여기에 Delay 이후의 로직 작성
			this->Destroy(); 
		}), 0.05f, false);
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
		Bird = Cast<ABase_Bird>(OtherActor);
		if (Bird)
		{
			OtherSpeed= Bird->CustomVelocity.Size();
			// UE_LOG(LogTemp, Log, TEXT("새 속도: %f"), OtherSpeed);
		}
	}
	
	if (OtherActor->IsA(ABase_Bird::StaticClass()))
	{
		SelectedThreshold = BirdThreshold;
		// UE_LOG(LogTemp, Log, TEXT("새..."));
	}
	
	float ImpactSpeed = FMath::Abs(OtherSpeed - this->GetVelocity().Size());
	
	// UE_LOG(LogTemp, Warning, TEXT("ImpulseSize: %f, SelectedThreshold: %f"), ImpactSpeed, SelectedThreshold);
	
	if (ImpactSpeed > SelectedThreshold)
	{
		CalculatedDamage = 50 + (ImpactSpeed - SelectedThreshold) / 10;
		if (SelectedThreshold == BirdThreshold)
		{
			CalBirdDamage();
		}

		// 나 자신에게 데미지를 입힘 (내가 부딪혀서 아픔)
		UGameplayStatics::ApplyDamage(this, CalculatedDamage, nullptr, OtherActor, UDamageType::StaticClass());
	}
}


void ABaseBlock::CalBirdDamage()
{
	CalculatedDamage *= 1;
}

void ABaseBlock::BeforeBlockDestory()
{
	
	// 2. 캐스팅에 성공했다면?
	if (GameState)
	{
		// UE_LOG(LogTemp, Warning, TEXT("점수추가 %f"), BlockPrice);
		GameState->AddScore(BlockPrice);
	}
	
	OnScoreChanged.Broadcast(BlockPrice);
	
	UE_LOG(LogTemp, Warning, TEXT("파괴 %s"), *GetName());
}
