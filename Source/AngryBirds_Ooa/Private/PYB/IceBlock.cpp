// Fill out your copyright notice in the Description page of Project Settings.


#include "PYB/IceBlock.h"

#include "Bomb_Bird.h"
#include "Speed_Bird.h"

AIceBlock::AIceBlock()
{
	BaseDamage = 50.0f;
	if (bodyMeshComp)
	{
		bodyMeshComp->SetLinearDamping(0.1f);
		bodyMeshComp->SetAngularDamping(0.5f);
		bodyMeshComp->GetBodyInstance()->SetMassOverride(150.0f);
		
		ConstructorHelpers::FObjectFinder<UMaterial> IceMat(TEXT("/Script/Engine.Material'/Game/PYB/Materials/Mat_Ice.Mat_Ice'"));
		if (IceMat.Succeeded())
		{
			bodyMeshComp->SetMaterial(0, IceMat.Object);
		}
	}
	ConstructorHelpers::FObjectFinder<USoundBase> tempHitSound(TEXT("/Script/Engine.SoundWave'/Game/PYB/Sounds/glass_hit.glass_hit'"));
	if (tempHitSound.Succeeded())
	{
		HitSound = tempHitSound.Object;
	}
	ConstructorHelpers::FObjectFinder<USoundBase> tempBreakSound(TEXT("/Script/Engine.SoundWave'/Game/PYB/Sounds/glass_broke.glass_broke'"));
	if (tempBreakSound.Succeeded())
	{
		BreakSound = tempBreakSound.Object;
	}
	ConstructorHelpers::FObjectFinder<UParticleSystem> tempHitParticle(TEXT("/Script/Engine.ParticleSystem'/Game/Realistic_Starter_VFX_Pack_Vol2/Particles/Hit/P_Paper.P_Paper'"));
	if (tempHitParticle.Succeeded())
	{
		HitParticle = tempHitParticle.Object;
	}
	ConstructorHelpers::FObjectFinder<UParticleSystem> tempBreakParticle(TEXT("/Script/Engine.ParticleSystem'/Game/Realistic_Starter_VFX_Pack_Vol2/Particles/Destruction/P_Destruction_Metal.P_Destruction_Metal'"));
	if (tempBreakParticle.Succeeded())
	{
		BreakParticle = tempBreakParticle.Object;
	}
}

void AIceBlock::CalBirdDamage()
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
