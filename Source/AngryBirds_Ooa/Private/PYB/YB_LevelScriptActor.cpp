// Fill out your copyright notice in the Description page of Project Settings.


#include "PYB/YB_LevelScriptActor.h"

void AYB_LevelScriptActor::BeginPlay()
{
	Super::BeginPlay();
	PigNum = 0;
	StageInfo = "Tutorial Map";
}

void AYB_LevelScriptActor::ShowLevelInfo()
{
	UE_LOG(LogTemp, Warning, TEXT("PigNum: %d, StageInfo: %s"), PigNum, *StageInfo);
}
