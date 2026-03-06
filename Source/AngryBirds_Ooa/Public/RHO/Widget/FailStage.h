// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FailStage.generated.h"


class UButton;
/**
 * 
 */
UCLASS()
class ANGRYBIRDS_OOA_API UFailStage : public UUserWidget
{
	GENERATED_BODY()
protected:
	
	virtual void NativeConstruct() override;
	
	UPROPERTY(meta = (BindWidget))
	UButton* Btn_Retry;

	UPROPERTY(meta = (BindWidget))
	UButton* Btn_StageSelect;

	// 버튼 눌렀을 때 실행될 함수들
	UFUNCTION()
	void OnRetryClicked();

	UFUNCTION()
	void OnStageSelectClicked();
	
};
