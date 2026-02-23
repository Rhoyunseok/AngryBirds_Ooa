// Fill out your copyright notice in the Description page of Project Settings.


#include "PYB/TestBlock2.h"

// Sets default values
ATestBlock2::ATestBlock2()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ATestBlock2::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATestBlock2::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ATestBlock2::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

