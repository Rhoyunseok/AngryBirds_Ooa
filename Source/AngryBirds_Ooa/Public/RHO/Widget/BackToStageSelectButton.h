// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "BackToStageSelectButton.generated.h"

/**
 * 
 */
UCLASS()
class ANGRYBIRDS_OOA_API UBackToStageSelectButton : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_StageSelect;
	
	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_Quit;
	
private:
	void NativeConstruct() override;
	
	UFUNCTION()
	void OnBackToStageSelectClicked();
	
	UFUNCTION()
	void OnQuitClicked();
	
};
