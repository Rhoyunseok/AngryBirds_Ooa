#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SuccessStage.generated.h"

class UButton;
class UTextBlock;
class UStarWidget;

UCLASS()
class ANGRYBIRDS_OOA_API USuccessStage : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	// --- 위젯 바인딩 (이미지 UI 요소와 매칭) ---
	UPROPERTY(meta = (BindWidget))
	UButton* Btn_NextStage;

	UPROPERTY(meta = (BindWidget))
	UButton* Btn_Restart;

	UPROPERTY(meta = (BindWidget))
	UButton* Btn_StageSelect;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Txt_Score;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Txt_Stars;
    
	UPROPERTY(meta = (BindWidget))
	UStarWidget* WBP_StarWidget;
    
	// --- 설정 변수 (중요: 에디터에서 스테이지별로 다르게 설정 가능) ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Settings")
	FName NextLevelName;

	// --- 버튼 클릭 핸들러 ---
	UFUNCTION()
	void OnNextStageClicked();

	UFUNCTION()
	void OnRestartClicked();

	UFUNCTION()
	void OnStageSelectClicked();
};