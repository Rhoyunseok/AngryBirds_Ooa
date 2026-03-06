// Fill out your copyright notice in the Description page of Project Settings.


#include "RHO/Widget/SuccessStage.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "RHO/AngryBirdGameState.h"

void USuccessStage::NativeConstruct()
{
	Super::NativeConstruct();

	if (Btn_NextStage) Btn_NextStage->OnClicked.AddDynamic(this, &USuccessStage::OnNextStageClicked);
	if (Btn_StageSelect) Btn_StageSelect->OnClicked.AddDynamic(this, &USuccessStage::OnStageSelectClicked);


	AAngryBirdGameState* GameState = Cast<AAngryBirdGameState>(UGameplayStatics::GetGameState(this));
	if (GameState)
	{
		// 아까 GameState에서 계산한 별점 로직을 참고해서 가져옵니다.
		// 임시로 남은 새의 수를 띄우거나, 아까 계산해서 변수에 저장해둔 별 개수를 가져옵니다.
		int32 MyScore = GameState->GetCurrentScore();
		int32 MyStars = 1; 
			//GameState->GetCurrentStars();

		if (Txt_Score)
		{
			Txt_Score->SetText(FText::FromString(FString::Printf(TEXT("SCORE : %d"), MyScore)));
		}
		if (Txt_Stars)
		{
			Txt_Stars->SetText(FText::FromString(FString::Printf(TEXT("STARS : %d"), MyStars)));
		}
	}
}

void USuccessStage::OnNextStageClicked()
{
	// 헤더에서 지정해둔 다음 맵으로 이동!
	UGameplayStatics::OpenLevel(this, NextLevelName);
}

void USuccessStage::OnStageSelectClicked()
{
	UGameplayStatics::OpenLevel(this, FName("StageSelectMap")); 
}