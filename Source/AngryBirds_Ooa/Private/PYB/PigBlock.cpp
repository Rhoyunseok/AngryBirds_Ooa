// Fill out your copyright notice in the Description page of Project Settings.


#include "PYB/PigBlock.h"
#include "RHO/AngryBirdGameState.h"
#include "Kismet/GameplayStatics.h"


APigBlock::APigBlock()
{
	BirdThreshold = 10.0f;
	BlockHP = 1.0f;
	BlockPrice = 1000.0f;
	
	ConstructorHelpers::FObjectFinder<USoundBase> tempOinkSound(TEXT("/Script/Engine.SoundWave'/Game/PYB/Sounds/AnyConv_com__piglette_oink_a8.AnyConv_com__piglette_oink_a8'"));
	if (tempOinkSound.Succeeded())
	{
		OinkSound = tempOinkSound.Object;
	}
	ConstructorHelpers::FObjectFinder<USoundBase> tempBreakSound(TEXT("/Script/Engine.SoundWave'/Game/PYB/Sounds/AnyConv_com__piglette_damage_a4.AnyConv_com__piglette_damage_a4'"));
	if (tempBreakSound.Succeeded())
	{
		BreakSound = tempBreakSound.Object;
	}
	ConstructorHelpers::FObjectFinder<UParticleSystem> tempBreakParticle(TEXT("/Script/Engine.ParticleSystem'/Game/Realistic_Starter_VFX_Pack_Vol2/Particles/Smoke/P_Smoke_F.P_Smoke_F'"));
	if (tempBreakParticle.Succeeded())
	{
		BreakParticle = tempBreakParticle.Object;
	}
}

void APigBlock::BeginPlay()
{
	Super::BeginPlay();

	UGameplayStatics::PlaySoundAtLocation(
		GetWorld(),
		OinkSound,
		GetActorLocation(),
		1.0f,
		1.0f,
		0.0f,
		nullptr,
		nullptr
	);
}

void APigBlock::BeforeBlockDestory()
{
	Super::BeforeBlockDestory();
	UE_LOG(LogTemp, Warning, TEXT("PigDie"));
    
	// 2. 캐스팅에 성공했다면?
	if (GameState)
	{
		// 3. GameState에게 돼지가 죽었다고 알림 (여기서 UI 변경 방송이 자동으로 나갑니다!)
		GameState->DecreasePigCount();
        
		// (선택) 점수도 올리고 싶다면 여기서 호출하세요!
		// GameState->AddScore(500); 
	}

	// 돼지 자체의 델리게이트 실행 (이펙트나 사운드 재생용으로 쓰기 좋습니다)
	OnPigDestroyed.Broadcast();
}