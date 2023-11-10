// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "WheeledVehiclePawn.h"
#include "Interaction/IInteractableTarget.h"
#include "Interfaces/Vehicle.h"
#include "GTAVehicle.generated.h"

/**
 * 
 */
UCLASS(Abstract, BlueprintType)
class GTAVEHICLESRUNTIME_API AGTAVehicle : public AWheeledVehiclePawn, public IInteractableTarget, public IVehicle
{
	GENERATED_BODY()

public:
	AGTAVehicle(const FObjectInitializer& ObjectInitializer);
	
	virtual void GatherInteractionOptions(const FInteractionQuery& InteractQuery, FInteractionOptionBuilder& OptionBuilder) override;
	virtual void CustomizeInteractionEventData(const FGameplayTag& InteractionEventTag, FGameplayEventData& InOutEventData) override;

protected:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnVehicleEnter(AActor* PawnInstigator, ULyraAbilitySystemComponent* LyraASC);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnVehicleExit(AActor* CarInstigator, ULyraAbilitySystemComponent* LyraASC);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void Tick(float DeltaSeconds) override;

	virtual bool PrintVariables() const;

private:
	void Input_Move(const struct FInputActionValue& InputActionValue);
	UFUNCTION(Server, Reliable)
	void Server_Input_Move(FVector InputValue);
	
	void Input_Pitch(const FInputActionValue& InputActionValue);
	UFUNCTION(Server, Reliable)
	void Server_Input_Pitch(float InputValue);
	
	void Input_Roll(const FInputActionValue& InputActionValue);
	UFUNCTION(Server, Reliable)
	void Server_Input_Roll(float InputValue);
	
	void Input_Yaw(const FInputActionValue& InputActionValue);
	UFUNCTION(Server, Reliable)
	void Server_Input_Yaw(float InputValue);
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UVehicleExtensionComponent> VehicleExtensionComponent;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category="Dynamic Variables")
	FVector MoveInput;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category="Input")
	float YawInput;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category="Input")
	float PitchInput;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category="Input")
	float RollInput;
};
