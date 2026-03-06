// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SuccessStage.generated.h"


class UButton;
class UTextBlock; // 글씨를 띄울 텍스트 블록

/**
 * 
 */
UCLASS()
class ANGRYBIRDS_OOA_API USuccessStage : public UUserWidget
{
	GENERATED_BODY()
protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	UButton* Btn_NextStage;

	UPROPERTY(meta = (BindWidget))
	UButton* Btn_StageSelect;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Txt_Score;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Txt_Stars; 

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level")
	FName NextLevelName = "MapRho"; 
	
	UFUNCTION()
	void OnNextStageClicked();

	UFUNCTION()
	void OnStageSelectClicked();
	
};
