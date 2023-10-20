// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CommonTileView.h"
#include "GTATileView.generated.h"

/**
 * 
 */
UCLASS()
class GTAEXPLORERRUNTIME_API UGTATileView : public UCommonTileView
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void SetEntryWidget(TSubclassOf<UUserWidget> WidgetClass);

	UFUNCTION(BlueprintCallable)
	void SetHorizontalSpacing(float Value);

	UFUNCTION(BlueprintCallable)
	void SetVerticalSpacing(float Value);

protected:
	virtual void PostLoad() override;
};
