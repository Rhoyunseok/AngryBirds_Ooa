// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/DynamicMeshComponent.h" 
#include "SlingShot.generated.h"

UCLASS()
class ANGRYBIRDS_OOA_API ASlingShot : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASlingShot();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// 추가: 에디터에서 액터가 갱신될 때마다 실행되는 함수 (고무줄 형태 생성용)
	virtual void OnConstruction(const FTransform& Transform) override;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	// 새총의 중심점 (위치 기준)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Slingshot")
	USceneComponent* RootComp;

	// 단단한 새총 몸통 (Y자 형태)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Slingshot")
	UStaticMeshComponent* SlingshotBody;

	// 왼쪽 고무줄 (버텍스가 변할 다이내믹 메시)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Slingshot|Bands")
	UDynamicMeshComponent* LeftBand;

	// 오른쪽 고무줄
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Slingshot|Bands")
	UDynamicMeshComponent* RightBand;

	// 발사체를 놓는 가죽 파우치 추가!
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Slingshot|Pouch")
	UStaticMeshComponent* Pouch;

	
	
	// 조준 상태 변수들

	// 현재 플레이어가 파우치를 잡고 조준 중인지 여부
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Slingshot|State")
	bool bIsAiming;
	
	// 현재 파우치가 제자리로 탄성 있게 돌아가고 있는 중인지 여부
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Slingshot|State")
	bool bIsReturning;

	// 고무줄에 입힐 색상/재질(Material)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slingshot|Visuals") //UMaterialInterface 는 머티리얼 에셋을 참조할 때 사용하는 타입입니다. 에디터에서 머티리얼을 드롭해서 넣을 수 있게 해줍니다.
	UMaterialInterface* BandMaterial;

	// 마우스 휠로 조절할 파워 (파우치가 뒤로 당겨지는 Z 또는 X축 깊이)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slingshot|State")
	float PullPower;

	// 파우치의 원래 위치 (발사 후 되돌아가기 위함)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Slingshot|State")
	FVector DefaultPouchLocation;
	
	// 파우치가 돌아갈 때 사용할 속도 (물리 관성용)
	FVector PouchVelocity;

	// -----------------------------------------
	// 조준 업데이트 함수
	// -----------------------------------------
    
	// 매 프레임 마우스 커서 위치와 휠 값을 받아와 파우치 위치를 갱신하는 함수
	// APlayerController를 통해 플레이어의 마우스 정보를 가져옵니다.
	void UpdateAim(class APlayerController* PlayerController);
	
	// 고무줄(Dynamic Mesh)을 파우치 위치에 맞춰 다시 그리는 함수
	void UpdateBands();
	
	//발사체 관련 >_<
	// 블루프린트에서 어떤 새(액터)를 스폰할지 지정하는 클래스 변수
	UPROPERTY(EditAnywhere, Category = "Slingshot | Projectile")
	TSubclassOf<AActor> BirdClass;

	// 장전 함수
	UFUNCTION(BlueprintCallable, Category = "Slingshot")
	void LoadBird();

	// 발사(힘 전달) 함수
	UFUNCTION(BlueprintCallable, Category = "Slingshot")
	void FireBird();
	
	// 마우스 왼쪽 버튼을 눌렀을 때 호출할 함수
	UFUNCTION(BlueprintCallable, Category = "Slingshot")
	void PullString();

	// 마우스 왼쪽 버튼을 떼었을 때 호출할 함수
	UFUNCTION(BlueprintCallable, Category = "Slingshot")
	void ReleaseString();
	
	// 기존의 PullString, ReleaseString 아래에 추가해 주세요!
	UFUNCTION(BlueprintCallable, Category = "Slingshot")
	void IncreasePower();

	UFUNCTION(BlueprintCallable, Category = "Slingshot")
	void DecreasePower();
	
	// 키보드 q를 활용하여 base bird bool bAbilityUsed = false; 의 값을true로 바꿔준다
	UFUNCTION(BlueprintCallable, Category = "Slingshot")
	void TriggerBirdAbility();
	
	// MoveIgnoreActorAdd
	
	
private:
	// 현재 장전되어 파우치에 붙어있는 새를 기억하는 포인터
	UPROPERTY()
	AActor* CurrentBird;
	
	FVector StartAimLocation;
protected:
	// 새가 장전되어 있고 조준 가능한 상태인지 확인하는 변수
	bool bCanAim = true;           // 조준 가능 상태 플래그
	FTimerHandle ReloadTimerHandle; // 재장전용 타이머 핸들
	
};
