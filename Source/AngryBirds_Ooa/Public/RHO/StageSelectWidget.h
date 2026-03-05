// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StageSelectWidget.generated.h"

/**
 * 
 */
UCLASS()
class ANGRYBIRDS_OOA_API UStageSelectWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	
	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_BackToMainMenu;
	
	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_Tutorial;
	
	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_Stage1_1;
	
	virtual void NativeConstruct(); __override; // 위젯이 화면에 처음 생성될 때 실행되는 함수
	
private:
	
	UFUNCTION()
	void OnBackToMainMenuClicked();
	
	UFUNCTION()
	void OnTutorialClicked();
	
	UFUNCTION()
	void OnStage1_1Clicked();
	
	
	
	
};
