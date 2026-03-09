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
	
	// 클리어 별 표시를 위한 UI 요소 (예시로 텍스트로 바꿔봅시다)
	
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Tutorial_Stars;
	
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Stage1_1_Stars;

	// --- 미디어 재생을 위한 변수 ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Media")
	class UMediaPlayer* StageMediaPlayer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Media")
	class UMediaSource* StageVideoSource;

	// --- 추가된 사운드 변수 ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	class USoundBase* StageSelectBGM;

	UPROPERTY()
	class UAudioComponent* BGMComponent;
	
	
	

	virtual void NativeConstruct() override;
    
private:
	UFUNCTION() void OnBackToMainMenuClicked();
	UFUNCTION() void OnTutorialClicked();
	UFUNCTION() void OnStage1_1Clicked();
	
	void UpdateStageStarsUI(FString StageName, class UTextBlock* StarTextBlock);
};