// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "AngryBirdGameState.generated.h"

/**
 * gamestate 에서는
 * - 점수
 * - 현재 스테이지에서 사용 가능한 새의 종류 및 개수 - 레벨에서 가져옴
 * - 남은 새의 수
 * - 현재 스테이지의 별 개수
 * - 스테이지에 처음 배치된 총 돼지의 수 - 레벨에서 가져옴
 * - 현재 남아있는 돼지의 수 
 * - 현재 스테이지 이름 - 레벨에서 가져옴
 * - 새를 사용하면 이 정보는 
 * 를 관리할 예정
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnScoreChanged, int32, NewScore);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPigsChanged, int32, Remaining, int32, Total);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnBirdsChanged, int32, Remaining, int32, Total);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStarsChanged, int32, Stars);
// 현재 스테이지 정보도 필요하다면 여기에 선언하세요! (예: FOnStageInfoChanged)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStageInfoChanged, FString, NewStageInfo);

UCLASS()
class ANGRYBIRDS_OOA_API AAngryBirdGameState : public AGameStateBase
{
	GENERATED_BODY()
public:
	AAngryBirdGameState();
	
	// -- 델리게이트 인스턴스 (위젯이 Bind할 대상)
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnScoreChanged OnScoreChanged;
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnPigsChanged OnPigsChanged;
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnBirdsChanged OnBirdsChanged;
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnStarsChanged OnStarsChanged;
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnStageInfoChanged OnStageInfoChanged;
	
	// -- 데이터 조회 함수 (Getter) 추가 --
	UFUNCTION(BlueprintCallable, Category = "State")
	int32 GetCurrentScore() const { return CurrentScore; }
	UFUNCTION(BlueprintCallable, Category = "State")
	int32 GetRemainingPigs() const { return RemainingPigs; }
	UFUNCTION(BlueprintCallable, Category = "State")
	int32 GetTotalPigs() const { return TotalPigs; }
	UFUNCTION(BlueprintCallable, Category = "State")
	int32 GetRemainingBirds() const { return RemainingBirds; }
	UFUNCTION(BlueprintCallable, Category = "State")
	int32 GetTotalBirds() const { return TotalBirds; }
	UFUNCTION(BlueprintCallable, Category = "State")
	int32 GetCurrentStars() const { return CurrentStars; }
	UFUNCTION(BlueprintCallable, Category = "State")
	FString GetCurrentStageInfo() const { return CurrentStageInfo; }
	
	// -- 데이터 멤버 (State)
	UPROPERTY(VisibleAnywhere, Category = "State")
	int32 CurrentScore = 0;
	UPROPERTY(VisibleAnywhere, Category = "State")
	int32 TotalPigs = 0;
	UPROPERTY(VisibleAnywhere, Category = "State")
	int32 RemainingPigs = 0;
	UPROPERTY(VisibleAnywhere, Category = "State")
	int32 TotalBirds = 0;
	UPROPERTY(VisibleAnywhere, Category = "State")
	int32 RemainingBirds = 0;
	UPROPERTY(VisibleAnywhere, Category = "State")
	int32 CurrentStars = 0;
	UPROPERTY(VisibleAnywhere, Category = "State")
	FString CurrentStageInfo;
	
	// -- 데이터 수정 함수 (Setter) --
	void AddScore(int32 Amount);
	void SetTotalPigs(int32 Count);
	void DecreasePigCount();
	void SetTotalBirds(int32 Count);
	void UseBird();
	void UpdateStars(int32 NewStars);
	void SetStageInfo(const FString& NewStageInfo);
};
