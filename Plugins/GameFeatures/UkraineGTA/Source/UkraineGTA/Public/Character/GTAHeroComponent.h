// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/LyraHeroComponent.h"
#include "GTAHeroComponent.generated.h"

/**
 * 
 */
UCLASS()
class UKRAINEGTA_API UGTAHeroComponent : public ULyraHeroComponent
{
	GENERATED_BODY()

public:
	UGTAHeroComponent(const FObjectInitializer& ObjectInitializer);
};
