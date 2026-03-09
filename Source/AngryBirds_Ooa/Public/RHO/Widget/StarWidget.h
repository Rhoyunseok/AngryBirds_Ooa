#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StarWidget.generated.h"

UCLASS()
class ANGRYBIRDS_OOA_API UStarWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(meta = (BindWidget))
	class UImage* Star_1;

	UPROPERTY(meta = (BindWidget))
	class UImage* Star_2;

	UPROPERTY(meta = (BindWidget))
	class UImage* Star_3;

	// --- 애니메이션 바인딩 (WBP에서 이 이름으로 애니메이션을 만들어야 함) ---
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	class UWidgetAnimation* FillStar1Anim;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	class UWidgetAnimation* FillStar2Anim;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	class UWidgetAnimation* FillStar3Anim;
    
	// StarCount: 별 개수, bAnimate: 애니메이션 재생 여부
	void UpdateStars(int32 StarCount, bool bAnimate = true);

private:
	int32 CurrentDisplayedStars = 0;
};