// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AbilitySystem/LyraAbilitySet.h"
#include "Components/ActorComponent.h"
#include "Interaction/InteractionOption.h"
#include "Interfaces/Vehicle.h"
#include "VehicleExtensionComponent.generated.h"

class ULyraCameraMode;
class ULyraCameraComponent;
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

	bool Entered() const;

	FInteractionOption& GetInteractionOption();

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<ULyraCameraComponent> CameraComponent;

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

	TSubclassOf<ULyraCameraMode> DetermineCameraMode() const;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="GTA|Vehicle")
	TObjectPtr<ULyraAbilitySet> AbilitySetToGrant;

	UPROPERTY(EditDefaultsOnly, Getter, BlueprintReadWrite, Category="GTA|Vehicle")
	TObjectPtr<ULyraInputConfig> InputConfig;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="GTA|Vehicle")
	TObjectPtr<UInputMappingContext> MappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lyra|Camera")
	TSubclassOf<ULyraCameraMode> CameraMode;

	UPROPERTY(EditDefaultsOnly, Category="VehicleSettings")
	FInteractionOption Option;

private:
	FLyraAbilitySet_GrantedHandles LoadedAbilitySetHandles;
	TArray<uint32> LoadedBindHandles;
	TWeakObjectPtr<APawn> EnteredPawn;
	bool bEntered;
};
