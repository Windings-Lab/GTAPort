// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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
	UFUNCTION(BlueprintCallable, BlueprintPure)
	UUserWidget* GetWidgetFromItem(const UObject* Item);
};
