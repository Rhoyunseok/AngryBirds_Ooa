// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PYB/BaseBlock.h"
#include "PigBlock.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnPigDestroyedSignature);

UCLASS()
class ANGRYBIRDS_OOA_API APigBlock : public ABaseBlock
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;
	virtual void BeforeBlockDestory() override;
	
public:
	FOnPigDestroyedSignature OnPigDestroyed;
};
