#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FailStage.generated.h"

class UButton;

UCLASS()
class ANGRYBIRDS_OOA_API UFailStage : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
    
	UPROPERTY(meta = (BindWidget))
	UButton* Btn_Retry;

	UPROPERTY(meta = (BindWidget))
	UButton* Btn_StageSelect;

	UFUNCTION()
	void OnRetryClicked();

	UFUNCTION()
	void OnStageSelectClicked();
};