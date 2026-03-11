#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FailStage.generated.h"

class UButton;
class USoundBase;

UCLASS()
class ANGRYBIRDS_OOA_API UFailStage : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	
	// --- 사운드 설정 ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundBase* FailSound;
    
	UPROPERTY(meta = (BindWidget))
	UButton* Btn_Retry;

	UPROPERTY(meta = (BindWidget))
	UButton* Btn_StageSelect;

	UFUNCTION()
	void OnRetryClicked();

	UFUNCTION()
	void OnStageSelectClicked();
};