// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuWidget.generated.h"

/**
 * 
 */
UCLASS()
class ANGRYBIRDS_OOA_API UMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()
protected:
	// 블루프린트 UI에 배치할 버튼들과 이름을 완벽히 똑같이 맞춰야 연결됨!
	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_StageSelect;
	
	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_Options;

	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_Quit;

	// 위젯이 화면에 처음 생성될 때 실행되는 함수
	virtual void NativeConstruct() override;

private:
	// 버튼이 눌렸을 때 실행할 함수들 (반드시 UFUNCTION이어야 함)
	UFUNCTION()
	void OnStageSelectClicked();
	
	UFUNCTION()
	void OnOptionsClicked();

	UFUNCTION()
	void OnQuitClicked();
	
};
