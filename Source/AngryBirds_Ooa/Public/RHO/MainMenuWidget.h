#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
// 미디어 관련 헤더 추가
#include "MediaPlayer.h"
#include "MediaSource.h"
#include "Animation/WidgetAnimation.h"
#include "MainMenuWidget.generated.h"

UCLASS()
class ANGRYBIRDS_OOA_API UMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	// --- UI 컴포넌트 (BindWidget) ---
	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_StageSelect;
    
	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_Options;

	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_Quit;

	// --- UI 애니메이션 (BindWidgetAnim) ---
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* IntroAnim;

	// --- 미디어 관련 변수 (에디터에서 할당) ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Media")
	class UMediaPlayer* MainMenuMediaPlayer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Media")
	class UMediaSource* MainMenuVideoSource;

	// 위젯 생성 시 호출
	virtual void NativeConstruct() override;

private:
	// 버튼 클릭 이벤트 함수들
	UFUNCTION()
	void OnStageSelectClicked();
    
	UFUNCTION()
	void OnOptionsClicked();

	UFUNCTION()
	void OnQuitClicked();
};