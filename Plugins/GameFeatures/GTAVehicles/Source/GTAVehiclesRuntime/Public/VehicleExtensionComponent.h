// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "Interaction/InteractionOption.h"
#include "Interfaces/Vehicle.h"
#include "AbilitySystem/LyraAbilitySet.h"
#include "VehicleExtensionComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GTAVEHICLESRUNTIME_API UVehicleExtensionComponent : public UActorComponent, public IVehicle
{
	GENERATED_BODY()

public:	
	UVehicleExtensionComponent();

	void AddToNativeInputHandle(int32 Handle);

	FInteractionOption& GetInteractionOption();

protected:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnVehicleEnter(ALyraCharacter* PawnInstigator, USeatZoneComponent* SeatZone);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnVehicleExit(ALyraCharacter* PawnInstigator, USeatZoneComponent* SeatZone);

private:
	void SetPawnHidden(APawn* Pawn, bool Value);
	
	void SetAbilities(ALyraCharacter* Pawn, const USeatZoneComponent* SeatZone, bool Value);
	void SetInputs(ALyraCharacter* Pawn, const USeatZoneComponent* SeatZone, bool Value);

protected:
	UPROPERTY(EditDefaultsOnly, Category="VehicleSettings")
	FInteractionOption Option;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="VehicleSettings")
	TSubclassOf<class ULyraCameraMode> Camera;

private:
	
	FLyraAbilitySet_GrantedHandles LoadedAbilitySetHandles;
	TArray<uint32> LoadedBindHandles;
};
