// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/LyraCharacter.h"
#include "GTACharacter.generated.h"

/**
 * 
 */
UCLASS()
class UKRAINEGTA_API AGTACharacter : public ALyraCharacter
{
	GENERATED_BODY()

public:
	AGTACharacter();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Lyra|Character", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UGTAHeroComponent> GTAHeroComponent;
};
