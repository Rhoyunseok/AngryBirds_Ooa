// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PYB/BaseBlock.h"
#include "IceBlock.generated.h"

/**
 * 
 */
UCLASS()
class ANGRYBIRDS_OOA_API AIceBlock : public ABaseBlock
{
	GENERATED_BODY()
	
public:
	AIceBlock();
	
	UPROPERTY(EditAnywhere)
	float VSSpeedVal = 1.0f;
	UPROPERTY(EditAnywhere)
	float VSBombVal = 0.5f;
	
	virtual void CalBirdDamage() override;
};
