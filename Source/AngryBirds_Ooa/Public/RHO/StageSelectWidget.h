#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StageSelectWidget.generated.h"

UCLASS()
class ANGRYBIRDS_OOA_API UStageSelectWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// --- 버튼 (WBP 이름과 일치해야 함) ---
	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_BackToMainMenu;
    
	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_Tutorial;
    
	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_Stage1_1;

	// --- 미디어 재생을 위한 변수 추가 ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Media")
	class UMediaPlayer* StageMediaPlayer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Media")
	class UMediaSource* StageVideoSource;

	virtual void NativeConstruct() override;
    
private:
	UFUNCTION() void OnBackToMainMenuClicked();
	UFUNCTION() void OnTutorialClicked();
	UFUNCTION() void OnStage1_1Clicked();
};