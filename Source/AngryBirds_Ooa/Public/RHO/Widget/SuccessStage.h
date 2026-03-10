#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SuccessStage.generated.h"

class UButton;
class UTextBlock;

UCLASS()
class ANGRYBIRDS_OOA_API USuccessStage : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	// --- 위젯 바인딩 ---
	UPROPERTY(meta = (BindWidget))
	UButton* Btn_NextStage;

	UPROPERTY(meta = (BindWidget))
	UButton* Btn_Restart; // 추가됨

	UPROPERTY(meta = (BindWidget))
	UButton* Btn_StageSelect;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Txt_Score;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Txt_Stars;

	// --- 설정 변수 ---
	// 요청하신 대로 Stage1_1로 기본값 변경
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level")
	FName NextLevelName = TEXT("Stage1_1");

	// --- 버튼 클릭 핸들러 ---
	UFUNCTION()
	void OnNextStageClicked();

	UFUNCTION()
	void OnRestartClicked(); // 추가됨

	UFUNCTION()
	void OnStageSelectClicked();
};