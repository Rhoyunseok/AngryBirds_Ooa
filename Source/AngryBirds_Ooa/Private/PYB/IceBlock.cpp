// Fill out your copyright notice in the Description page of Project Settings.


#include "PYB/IceBlock.h"

AIceBlock::AIceBlock()
{
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
}