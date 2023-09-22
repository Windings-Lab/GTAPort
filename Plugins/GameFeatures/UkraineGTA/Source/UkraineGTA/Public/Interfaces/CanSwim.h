// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/Interface.h"
#include "CanSwim.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UCanSwim : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class UKRAINEGTA_API ICanSwim
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual bool IsSwimming() const;
	virtual bool IsInWater() const;
	virtual bool IsTouchingGroundInWater() const;
	virtual bool CanSwimUp() const;
};
