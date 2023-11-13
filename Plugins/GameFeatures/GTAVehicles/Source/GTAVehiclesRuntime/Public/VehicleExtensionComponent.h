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
	const class ULyraInputConfig* GetInputConfig() const;

	FInteractionOption& GetInteractionOption();

	bool WithDriver() const;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<class ULyraCameraComponent> CameraComponent;

protected:
	virtual void BeginPlay() override;

protected:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnVehicleEnter(AActor* PawnInstigator, ULyraAbilitySystemComponent* LyraASC);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnVehicleExit(AActor* PawnInstigator, ULyraAbilitySystemComponent* LyraASC);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	TSubclassOf<class ULyraCameraMode> DetermineCameraMode() const;
	void SetPawnHidden(APawn* Pawn, bool Value);
	
	void SetAbilities(APawn* Pawn, ULyraAbilitySystemComponent* LyraASC, bool Value);
	void SetInputs(APawn* Pawn, ULyraAbilitySystemComponent* LyraASC, bool Value);
	void SetAttachments(APawn* Pawn, ULyraAbilitySystemComponent* LyraASC, bool Value);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="GTA|Vehicle")
	TObjectPtr<class ULyraAbilitySet> AbilitySetToGrant;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="GTA|Vehicle")
	TObjectPtr<ULyraInputConfig> InputConfig;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="GTA|Vehicle")
	TObjectPtr<class UInputMappingContext> MappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lyra|Camera")
	TSubclassOf<ULyraCameraMode> CameraMode;

	UPROPERTY(EditDefaultsOnly, Category="VehicleSettings")
	FInteractionOption Option;

private:
	UPROPERTY(Replicated)
	bool bDriverEntered;
	
	FVector EnteredRelativePosition;
	
	FLyraAbilitySet_GrantedHandles LoadedAbilitySetHandles;
	TArray<uint32> LoadedBindHandles;
};
