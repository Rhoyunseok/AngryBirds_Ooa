#include "RHO/Widget/StarWidget.h"
#include "Components/Image.h"
#include "Animation/WidgetAnimation.h"

void UStarWidget::UpdateStars(int32 StarCount, bool bAnimate)
{
	if (StarCount == CurrentDisplayedStars) return;

	// 기본 투명도 설정 (연한 상태)
	auto SetInitialOpacity = [this](int32 Count) {
		if (Star_1) Star_1->SetRenderOpacity(Count >= 1 ? 1.0f : 0.2f);
		if (Star_2) Star_2->SetRenderOpacity(Count >= 2 ? 1.0f : 0.2f);
		if (Star_3) Star_3->SetRenderOpacity(Count >= 3 ? 1.0f : 0.2f);
	};

	if (bAnimate)
	{
		// 새로 채워져야 할 별들에 대해서만 애니메이션 재생
		if (StarCount >= 1 && CurrentDisplayedStars < 1 && FillStar1Anim) PlayAnimation(FillStar1Anim);
		if (StarCount >= 2 && CurrentDisplayedStars < 2 && FillStar2Anim) PlayAnimation(FillStar2Anim);
		if (StarCount >= 3 && CurrentDisplayedStars < 3 && FillStar3Anim) PlayAnimation(FillStar3Anim);
	}
	else
	{
		// 애니메이션 없이 즉시 투명도만 조절 (스테이지 선택창용)
		SetInitialOpacity(StarCount);
	}

	CurrentDisplayedStars = StarCount;
}