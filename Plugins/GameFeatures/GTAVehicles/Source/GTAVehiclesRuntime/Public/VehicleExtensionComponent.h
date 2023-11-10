// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AbilitySystem/LyraAbilitySet.h"
#include "Components/ActorComponent.h"
#include "Interaction/InteractionOption.h"
#include "Interfaces/Vehicle.h"
#include "VehicleExtensionComponent.generated.h"

class UInputAction;
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

	static bool IsDriver(ULyraAbilitySystemComponent* LyraASC);
	static bool IsPassenger(ULyraAbilitySystemComponent* LyraASC);

	void AddToNativeInputHandle(int32 Handle);
	const ULyraInputConfig* GetInputConfig() const;

	FInteractionOption& GetInteractionOption();

	bool DriverEntered() const;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<ULyraCameraComponent> CameraComponent;

protected:
	virtual void BeginPlay() override;

protected:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnVehicleEnter(AActor* PawnInstigator, ULyraAbilitySystemComponent* LyraASC);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnVehicleExit(AActor* PawnInstigator, ULyraAbilitySystemComponent* LyraASC);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	void Input_AbilityInputTagPressed(FGameplayTag InputTag);
	void Input_AbilityInputTagReleased(FGameplayTag InputTag);

	TSubclassOf<ULyraCameraMode> DetermineCameraMode() const;
	void SetPawnHidden(APawn* Pawn, bool Value);
	
	void SetAbilities(APawn* Pawn, ULyraAbilitySystemComponent* LyraASC, bool Value);
	void SetInputs(APawn* Pawn, ULyraAbilitySystemComponent* LyraASC, bool Value);
	void SetAttachments(APawn* Pawn, ULyraAbilitySystemComponent* LyraASC, bool Value);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="GTA|Vehicle")
	TObjectPtr<ULyraAbilitySet> AbilitySetToGrant;

	UPROPERTY(EditDefaultsOnly, Getter, BlueprintReadWrite, Category="GTA|Vehicle")
	TObjectPtr<ULyraInputConfig> InputConfig;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="GTA|Vehicle")
	TObjectPtr<UInputMappingContext> DriverMappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lyra|Camera")
	TSubclassOf<ULyraCameraMode> CameraMode;

	UPROPERTY(EditDefaultsOnly, Category="VehicleSettings")
	FInteractionOption Option;

private:
	FVector EnteredRelativePosition;
	
	FLyraAbilitySet_GrantedHandles LoadedAbilitySetHandles;
	TArray<uint32> LoadedBindHandles;

	UPROPERTY(Replicated)
	TObjectPtr<APawn> Driver;

	UPROPERTY(Replicated)
	TObjectPtr<APawn> Passenger;

	UPROPERTY()
	TObjectPtr<UInputAction> VehicleExitInput;

	UPROPERTY()
	TObjectPtr<UInputMappingContext> PassengerMappingContext;
};
