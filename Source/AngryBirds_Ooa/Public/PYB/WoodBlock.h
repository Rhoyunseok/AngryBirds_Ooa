// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PYB/BaseBlock.h"
#include "WoodBlock.generated.h"

/**
 * 
 */
UCLASS()
class ANGRYBIRDS_OOA_API AWoodBlock : public ABaseBlock
{
	GENERATED_BODY()
	
public:
	AWoodBlock();
	
	UPROPERTY(EditAnywhere)
	float VSSpeedVal = 1.5f;
	UPROPERTY(EditAnywhere)
	float VSBombVal = 0.5f;
	
	virtual void CalBirdDamage() override;
};
