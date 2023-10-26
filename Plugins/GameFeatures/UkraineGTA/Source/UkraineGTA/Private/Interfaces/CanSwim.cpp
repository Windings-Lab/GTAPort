// Fill out your copyright notice in the Description page of Project Settings.


#include "Interfaces/CanSwim.h"

// Add default functionality here for any ICanSwim functions that are not pure virtual.
bool ICanSwim::IsSwimming() const
{
	return false;
}

bool ICanSwim::AffectedByWater() const
{
	return false;
}

bool ICanSwim::IsTouchingGroundInWater() const
{
	return false;
}

bool ICanSwim::IsUnderWater() const
{
	return false;
}
