// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Player/LyraPlayerState.h"
#include "GTAPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class UKRAINEGTA_API AGTAPlayerState : public ALyraPlayerState
{
	GENERATED_BODY()

	AGTAPlayerState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};
