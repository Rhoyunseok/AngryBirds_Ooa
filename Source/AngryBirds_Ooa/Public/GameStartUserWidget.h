// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameStartUserWidget.generated.h"
class UButton;
/**
 * 
 */
UCLASS()
class ANGRYBIRDS_OOA_API UGameStartUserWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	
	//위젯이 생성될 때 호출되는 초기화 함수
	virtual void NativeConstruct() override;
	
	//UMG 디자이너에 있는 버튼과 이름을 똑같이 맞춰야 한다 
	//BindWidget는 UMG에서 만든 버튼과 연결해주는 역할을 한다 
	//GameStartButton은 UMG에서 만든 버튼의 이름과 일치해야 합니다.
	UPROPERTY(meta = (BindWidget))
	UButton* GameStartButton;
	
	//버튼 클릭 시 실행될 함수
	UFUNCTION()
	void OnGameStartButtonClicked();
};
