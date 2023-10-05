// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GTAWheeledVehicle.h"
#include "BicycleVehicle.generated.h"

/**
 * 
 */
UCLASS()
class GTAVEHICLESRUNTIME_API ABicycleVehicle : public AGTAWheeledVehicle
{
	GENERATED_BODY()

protected:
	virtual void OnVehicleEnter_Implementation(AActor* CarInstigator, ULyraAbilitySystemComponent* LyraASC) override;
	virtual void OnVehicleExit_Implementation(AActor* CarInstigator, ULyraAbilitySystemComponent* LyraASC) override;
};
