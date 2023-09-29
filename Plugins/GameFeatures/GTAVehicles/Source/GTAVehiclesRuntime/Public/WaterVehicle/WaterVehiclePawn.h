// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Pawn.h"
#include "Interaction/IInteractableTarget.h"
#include "Interfaces/Vehicle.h"
#include "WaterVehiclePawn.generated.h"

class UVehicleExtensionComponent;
class ULyraCameraComponent;
struct FInputActionValue;
class ULyraCameraMode;

UCLASS()
class GTAVEHICLESRUNTIME_API AWaterVehiclePawn : public APawn, public IInteractableTarget, public IVehicle
{
	GENERATED_BODY()

public:
	AWaterVehiclePawn();

	virtual void GatherInteractionOptions(const FInteractionQuery& InteractQuery, FInteractionOptionBuilder& OptionBuilder) override;
	virtual void CustomizeInteractionEventData(const FGameplayTag& InteractionEventTag, FGameplayEventData& InOutEventData) override;

protected:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnVehicleEnter(AActor* CarInstigator, ULyraAbilitySystemComponent* LyraASC);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnVehicleExit(AActor* CarInstigator, ULyraAbilitySystemComponent* LyraASC);

	virtual void Tick(float DeltaSeconds) override;

private:
	void Input_Move(const FInputActionValue& InputActionValue);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<class UStaticMeshComponent> WaterVehicleMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UVehicleExtensionComponent> VehicleExtensionComponent;
};
