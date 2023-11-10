// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GTAVehicle.h"
#include "BoatVehicle.generated.h"

UCLASS()
class GTAVEHICLESRUNTIME_API ABoatVehicle : public AGTAVehicle
{
	GENERATED_BODY()

public:
	ABoatVehicle(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void Tick(float DeltaSeconds) override;
};
