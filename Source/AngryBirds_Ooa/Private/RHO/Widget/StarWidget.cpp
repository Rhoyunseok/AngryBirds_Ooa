#include "RHO/Widget/StarWidget.h"
#include "Components/Image.h"
#include "Animation/WidgetAnimation.h"

void UStarWidget::UpdateStars(int32 StarCount, bool bAnimate)
{
	// 1. 일단 모든 별을 연하게(0.2) 초기화
	if (Star_1) Star_1->SetRenderOpacity(0.2f);
	if (Star_2) Star_2->SetRenderOpacity(0.2f);
	if (Star_3) Star_3->SetRenderOpacity(0.2f);

	if (StarCount <= 0) return;

	if (bAnimate)
	{
		// 2. 애니메이션 실행 및 '최종 상태' 강제 고정
		// PlayAnimation을 호출해도, 코드에서 직접 Opacity를 1.0으로 박아버리면 
		// 애니메이션이 끝나고 흐려지는 현상을 막을 수 있습니다.
		if (StarCount >= 1 && FillStar1Anim) 
		{
			PlayAnimation(FillStar1Anim);
			Star_1->SetRenderOpacity(1.0f); 
		}
		if (StarCount >= 2 && FillStar2Anim) 
		{
			PlayAnimation(FillStar2Anim);
			Star_2->SetRenderOpacity(1.0f);
		}
		if (StarCount >= 3 && FillStar3Anim) 
		{
			PlayAnimation(FillStar3Anim);
			Star_3->SetRenderOpacity(1.0f);
		}
	}
	else
	{
		// 애니메이션 없이 즉시 표시
		if (Star_1) Star_1->SetRenderOpacity(StarCount >= 1 ? 1.0f : 0.2f);
		if (Star_2) Star_2->SetRenderOpacity(StarCount >= 2 ? 1.0f : 0.2f);
		if (Star_3) Star_3->SetRenderOpacity(StarCount >= 3 ? 1.0f : 0.2f);
	}

	CurrentDisplayedStars = StarCount;
}