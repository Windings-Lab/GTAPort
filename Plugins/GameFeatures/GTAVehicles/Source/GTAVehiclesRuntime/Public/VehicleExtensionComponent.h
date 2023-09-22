// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AbilitySystem/LyraAbilitySet.h"
#include "Components/ActorComponent.h"
#include "Interfaces/Vehicle.h"
#include "VehicleExtensionComponent.generated.h"

class UInputMappingContext;
class ULyraInputConfig;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GTAVEHICLESRUNTIME_API UVehicleExtensionComponent : public UActorComponent, public IVehicle
{
	GENERATED_BODY()

public:	
	UVehicleExtensionComponent();

	void AddToNativeInputHandle(int32 Handle);
	
	const ULyraInputConfig* GetInputConfig() const;

protected:
	virtual void BeginPlay() override;

protected:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnVehicleEnter(AActor* CarInstigator, ULyraAbilitySystemComponent* LyraASC);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnVehicleExit(AActor* CarInstigator, ULyraAbilitySystemComponent* LyraASC);

private:
	void Input_AbilityInputTagPressed(FGameplayTag InputTag);
	void Input_AbilityInputTagReleased(FGameplayTag InputTag);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="GTA|Vehicle")
	TObjectPtr<ULyraAbilitySet> AbilitySetToGrant;

	UPROPERTY(EditDefaultsOnly, Getter, BlueprintReadWrite, Category="GTA|Vehicle")
	TObjectPtr<ULyraInputConfig> InputConfig;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="GTA|Vehicle")
	TObjectPtr<UInputMappingContext> MappingContext;

private:
	FLyraAbilitySet_GrantedHandles LoadedAbilitySetHandles;
	TArray<uint32> LoadedBindHandles;
	TWeakObjectPtr<APawn> EnteredPawn;
};
