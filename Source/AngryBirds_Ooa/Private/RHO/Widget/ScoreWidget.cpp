// Fill out your copyright notice in the Description page of Project Settings.


#include "RHO/Widget/ScoreWidget.h"
#include "Components/TextBlock.h"


void UScoreWidget::UpdateScore(int32 NewScore)
{
	if (Score)
	{
		Score->SetText(FText::AsNumber(NewScore));
	}
}
