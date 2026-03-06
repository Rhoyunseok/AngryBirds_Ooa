#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuWidget.generated.h"

UCLASS()
class ANGRYBIRDS_OOA_API UMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	// 버튼 (이름이 WBP 내의 버튼 이름과 같아야 함)
	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_StageSelect;
    
	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_Options;

	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_Quit;

	// 미디어 관련 (에디터에서 할당)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Media")
	class UMediaPlayer* MainMenuMediaPlayer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Media")
	class UMediaSource* MainMenuVideoSource;

	virtual void NativeConstruct() override;

private:
	UFUNCTION() void OnStageSelectClicked();
	UFUNCTION() void OnOptionsClicked();
	UFUNCTION() void OnQuitClicked();
};