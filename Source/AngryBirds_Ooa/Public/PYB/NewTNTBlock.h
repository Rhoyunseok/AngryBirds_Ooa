// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PYB/BaseBlock.h"
#include "NewTNTBlock.generated.h"

/**
 * 
 */
UCLASS()
class ANGRYBIRDS_OOA_API ANewTNTBlock : public ABaseBlock
{
	GENERATED_BODY()
	
public:
	ANewTNTBlock();
	
protected:
	virtual void BeginPlay() override;
	virtual void BeforeBlockDestory() override;
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ExplosionRadius = 500.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ExplosionStrength = 1000.0f;
	
	UFUNCTION()
	void Explode();

};
