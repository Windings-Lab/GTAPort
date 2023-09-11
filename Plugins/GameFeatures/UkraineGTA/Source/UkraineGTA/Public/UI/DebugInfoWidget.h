// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CommonActivatableWidget.h"
#include "CommonUserWidget.h"
#include "UI/LyraActivatableWidget.h"
#include "DebugInfoWidget.generated.h"

class UCommonRichTextBlock;
/**
 * 
 */
UCLASS()
class UKRAINEGTA_API UDebugInfoWidget : public ULyraActivatableWidget
{
	GENERATED_BODY()

private:
	UFUNCTION(BlueprintCallable)
	void TickCPP(float DeltaSeconds);

private:
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget, AllowPrivateAccess=true))
	UCommonRichTextBlock* Coordinates;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget, AllowPrivateAccess=true))
	UCommonRichTextBlock* CharacterSpeed;
};
