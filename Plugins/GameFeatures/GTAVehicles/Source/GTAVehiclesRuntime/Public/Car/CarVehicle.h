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

public:
	ACarVehicle(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
};
