#include "RHO/Widget/FailStage.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

void UFailStage::NativeConstruct()
{
	Super::NativeConstruct();

	if (Btn_Retry)
	{
		Btn_Retry->OnClicked.AddDynamic(this, &UFailStage::OnRetryClicked);
	}
	if (Btn_StageSelect)
	{
		Btn_StageSelect->OnClicked.AddDynamic(this, &UFailStage::OnStageSelectClicked);
	}
}

void UFailStage::OnRetryClicked()
{
	// 현재 레벨을 다시 로드하여 재시도 구현
	FString CurrentLevel = UGameplayStatics::GetCurrentLevelName(this);
	UGameplayStatics::OpenLevel(this, FName(*CurrentLevel));
}

void UFailStage::OnStageSelectClicked()
{
	UGameplayStatics::OpenLevel(this, FName("StageSelect")); 
}