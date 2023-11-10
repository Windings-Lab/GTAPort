// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GTAVehicle.h"
#include "BicycleVehicle.generated.h"

/**
 * 
 */
UCLASS()
class GTAVEHICLESRUNTIME_API ABicycleVehicle : public AGTAVehicle
{
	GENERATED_BODY()

public:
	ABicycleVehicle(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void Tick(float DeltaSeconds) override;
};
