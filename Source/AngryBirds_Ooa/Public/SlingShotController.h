#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SlingShotController.generated.h"

// ★ 전방 선언 (Forward Declaration)
// 컴파일러에게 "나중에 ASlingShot이라는 클래스를 쓸 거야"라고 미리 알려줍니다.
class ASlingShot;

UCLASS()
class ANGRYBIRDS_OOA_API ASlingShotController : public APlayerController
{
	GENERATED_BODY()

public:
	ASlingShotController();

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

	// 조작 함수들
	void StartAiming();
	void StopAiming();
	void AdjustPower(float AxisValue);

	// 현재 조종 중인 새총 참조
	// 에러 났던 부분: ASlingShot 앞에 class를 붙이거나 위에 전방 선언이 있어야 합니다.
	UPROPERTY()
	ASlingShot* CurrentSlingshot;
};