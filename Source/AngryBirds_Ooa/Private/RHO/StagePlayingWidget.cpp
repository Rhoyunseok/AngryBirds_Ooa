#include "RHO/StagePlayingWidget.h"
#include "RHO/AngryBirdGameState.h"

// 하위 위젯들의 헤더를 반드시 포함해야 합니다 (경로는 프로젝트에 맞게 수정)
#include "RHO/Widget/ScoreWidget.h"
#include "RHO/Widget/PigWidget.h"
#include "RHO/Widget/BirdWidget.h"
#include "RHO/Widget/StarWidget.h"
#include "RHO/Widget/StageInfoWidget.h"

void UStagePlayingWidget::NativeConstruct()
{
    Super::NativeConstruct();
    
    // GameState 가져오기
    AAngryBirdGameState* GameState = GetWorld() ? GetWorld()->GetGameState<AAngryBirdGameState>() : nullptr;
    if (GameState) {
       // GameState의 Delegate에 함수 바인딩
       GameState->OnScoreChanged.AddDynamic(this, &UStagePlayingWidget::OnScoreChanged);
       GameState->OnPigsChanged.AddDynamic(this, &UStagePlayingWidget::OnPigsChanged);
       GameState->OnBirdsChanged.AddDynamic(this, &UStagePlayingWidget::OnBirdsChanged);
       GameState->OnStarsChanged.AddDynamic(this, &UStagePlayingWidget::OnStarsChanged);
        GameState->OnStageInfoChanged.AddDynamic(this, &UStagePlayingWidget::OnStageInfoChanged);
       
       // 초기 UI 업데이트 (게임 시작 시점의 정보로 UI를 초기화)
       // GameState에 GetCurrentStars() 등의 Getter 함수가 선언되어 있다고 가정합니다.
       UpdateAllUI(
           GameState->GetCurrentScore(), 
           GameState->GetRemainingPigs(), GameState->GetTotalPigs(), 
           GameState->GetRemainingBirds(), GameState->GetTotalBirds(), // 추가된 새 정보
           GameState->GetCurrentStars() // 별 정보 Getter
       );
    }
}

void UStagePlayingWidget::UpdateAllUI(int32 Score, int32 RemainingPigs, int32 TotalPigs, int32 RemainingBirds, int32 TotalBirds, int32 Stars)
{
    // 각 개별 업데이트 함수를 재사용하여 코드를 깔끔하게 만듭니다.
    OnScoreChanged(Score);
    OnPigsChanged(RemainingPigs, TotalPigs);
    OnBirdsChanged(RemainingBirds, TotalBirds);
    OnStarsChanged(Stars);
}

void UStagePlayingWidget::OnScoreChanged(int32 NewScore)
{
    // ScoreWidget이 정상적으로 바인딩(생성) 되었는지 확인 후 점수 업데이트
    if (WBP_ScoreWidget)
    {
        WBP_ScoreWidget->UpdateScore(NewScore); // ScoreWidget 내부에 UpdateScore 함수가 있어야 함
    }
}

void UStagePlayingWidget::OnPigsChanged(int32 RemainingPigs, int32 TotalPigs)
{
    if (WBP_PigWidget)
    {
        WBP_PigWidget->UpdatePigCount(RemainingPigs, TotalPigs); // UPigWidget 내부에 구현 필요
    }
}

void UStagePlayingWidget::OnBirdsChanged(int32 RemainingBirds, int32 TotalBirds)
{
    if (WBP_BirdWidget)
    {
        WBP_BirdWidget->UpdateBirdCount(RemainingBirds, TotalBirds); // UBirdWidget 내부에 구현 필요
    }
}

void UStagePlayingWidget::OnStarsChanged(int32 Stars)
{
    if (WBP_StarWidget)
    {
        WBP_StarWidget->UpdateStars(Stars); // UStarWidget 내부에 구현 필요
    }
}

void UStagePlayingWidget::OnStageInfoChanged(FString NewStageInfo)
{
    // Level Script Actor에서 전달받은 스테이지 정보를 UI에 표시하는 로직을 여기에 구현할 수 있습니다.
    if (WBP_StageInfoWidget)
    {
        WBP_StageInfoWidget->UpdateStageInfo(NewStageInfo); // UStageInfoWidget 내부에 구현 필요
    }
}
