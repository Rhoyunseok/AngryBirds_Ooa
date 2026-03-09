// Fill out your copyright notice in the Description page of Project Settings.


#include "PYB/WoodBlock.h"

#include "Base_Bird.h"
#include "Bomb_Bird.h"
#include "Speed_Bird.h"

AWoodBlock::AWoodBlock()
{
	if (bodyMeshComp)
	{
		bodyMeshComp->SetLinearDamping(0.1f);
		bodyMeshComp->SetAngularDamping(0.5f);
		bodyMeshComp->GetBodyInstance()->SetMassOverride(100.0f);
		
		ConstructorHelpers::FObjectFinder<UMaterial> WoodMat(TEXT("/Script/Engine.Material'/Game/PYB/Materials/Mat_Wood.Mat_Wood''"));
		if (WoodMat.Succeeded())
		{
			bodyMeshComp->SetMaterial(0, WoodMat.Object);
		}
	}
}

void AWoodBlock::CalBirdDamage()
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
