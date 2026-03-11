// Fill out your copyright notice in the Description page of Project Settings.


#include "PYB/RockBlock.h"

#include "Bomb_Bird.h"
#include "Speed_Bird.h"

ARockBlock::ARockBlock()
{
	BaseDamage = 10.0f;
	if (bodyMeshComp)
	{
		bodyMeshComp->SetLinearDamping(0.1f);
		bodyMeshComp->SetAngularDamping(0.5f);
		bodyMeshComp->GetBodyInstance()->SetMassOverride(200.0f);
		
		ConstructorHelpers::FObjectFinder<UMaterial> RockMat(TEXT("/Script/Engine.Material'/Game/PYB/Materials/Mat_Rock.Mat_Rock'"));
		if (RockMat.Succeeded())
		{
			bodyMeshComp->SetMaterial(0, RockMat.Object);
		}
	}
	ConstructorHelpers::FObjectFinder<USoundBase> tempHitSound(TEXT("/Script/Engine.SoundWave'/Game/PYB/Sounds/stone_hit.stone_hit'"));
	if (tempHitSound.Succeeded())
	{
		HitSound = tempHitSound.Object;
	}
	ConstructorHelpers::FObjectFinder<USoundBase> tempBreakSound(TEXT("/Script/Engine.SoundWave'/Game/PYB/Sounds/stone_broke.stone_broke'"));
	if (tempBreakSound.Succeeded())
	{
		BreakSound = tempBreakSound.Object;
	}
	ConstructorHelpers::FObjectFinder<UParticleSystem> tempHitParticle(TEXT("/Script/Engine.ParticleSystem'/Game/Realistic_Starter_VFX_Pack_Vol2/Particles/Hit/P_Brick.P_Brick'"));
	if (tempHitParticle.Succeeded())
	{
		HitParticle = tempHitParticle.Object;
	}
	ConstructorHelpers::FObjectFinder<UParticleSystem> tempBreakParticle(TEXT("/Script/Engine.ParticleSystem'/Game/Realistic_Starter_VFX_Pack_Vol2/Particles/Destruction/P_Destruction_Concrete.P_Destruction_Concrete'"));
	if (tempBreakParticle.Succeeded())
	{
		BreakParticle = tempBreakParticle.Object;
	}
}

void ARockBlock::CalBirdDamage()
{
	Super::CalBirdDamage();
	if (Bird->IsA(ASpeed_Bird::StaticClass()))
	{
		UE_LOG(LogTemp, Warning, TEXT("Bird is a Speed_Bird"));
		CalculatedDamage *= VSSpeedVal;
	}
	else if (Bird->IsA(ABomb_Bird::StaticClass()))
	{
		UE_LOG(LogTemp, Warning, TEXT("Bird is a Bomb_Bird"));
		CalculatedDamage *= VSBombVal;
	}
}
