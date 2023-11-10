// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GTAVehicle.h"
#include "CarVehicle.generated.h"

/**
 * 
 */
UCLASS()
class GTAVEHICLESRUNTIME_API ACarVehicle : public AGTAVehicle
{
	GENERATED_BODY()

protected:
	virtual void Tick(float DeltaSeconds) override;
};
