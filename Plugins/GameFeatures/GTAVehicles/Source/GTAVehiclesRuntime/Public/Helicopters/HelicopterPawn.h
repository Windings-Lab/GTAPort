// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Pawn.h"
#include "Interaction/IInteractableTarget.h"
#include "Interfaces/Vehicle.h"
#include "HelicopterPawn.generated.h"

struct FInputActionValue;
class UVehicleExtensionComponent;
class ULyraCameraComponent;
class ULyraCameraMode;

UCLASS()
class GTAVEHICLESRUNTIME_API AHelicopterPawn : public APawn, public IInteractableTarget, public IVehicle
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AHelicopterPawn();

	virtual void GatherInteractionOptions(const FInteractionQuery& InteractQuery, FInteractionOptionBuilder& OptionBuilder) override;
	virtual void CustomizeInteractionEventData(const FGameplayTag& InteractionEventTag, FGameplayEventData& InOutEventData) override;

	UFUNCTION(BlueprintImplementableEvent)
	void K2_OnRotateBlades(float DeltaTime);

protected:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnVehicleEnter(AActor* CarInstigator, ULyraAbilitySystemComponent* LyraASC);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnVehicleExit(AActor* CarInstigator, ULyraAbilitySystemComponent* LyraASC);
	
	virtual void Tick(float DeltaTime) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
private:
	void Input_MoveUp(const FInputActionValue& InputActionValue);
	UFUNCTION(Server, Reliable)
	void Server_Input_MoveUp(float InputValue);
	
	void UpdateBladeRotationSpeed(float DT);
	void UpdateForceVector(float DT);
	void AddLinearForce(float DT);
	
	void Input_Yaw(const FInputActionValue& InputActionValue);
	UFUNCTION(Server, Reliable)
	void Server_Input_Yaw(float InputValue);
	
	void UpdateYaw(float CurrentTurnSpeed, float DT);
	
	void Input_Pitch(const FInputActionValue& InputActionValue);
	UFUNCTION(Server, Reliable)
	void Server_Input_Pitch(float InputValue);
	
	void UpdatePitch(float CurrentTurnSpeed, float DT);
	
	void Input_Roll(const FInputActionValue& InputActionValue);
	UFUNCTION(Server, Reliable)
	void Server_Input_Roll(float InputValue);
	
	void UpdateRoll(float CurrentTurnSpeed, float DT);
	
	float GetTurnSpeed();
	float GetUpForce();

	void PrintVariables();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> HelicopterMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UVehicleExtensionComponent> VehicleExtensionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Dynamic Variables")
	float CurrentBladeRotationSpeed;
	
	// Acceleration
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Dynamic Variables")
	FVector ForceVector;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Dynamic Variables")
	FVector CurrentYaw;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category="Dynamic Variables")
	float YawInput;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Dynamic Variables")
	FVector CurrentPitch;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Dynamic Variables")
	float CurrentPitchAngle;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category="Dynamic Variables")
	float PitchInput;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Dynamic Variables")
	FVector CurrentRoll;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Dynamic Variables")
	float CurrentRollAngle;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category="Dynamic Variables")
	float RollInput;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Dynamic Variables")
	float CurrentThrottle;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category="Dynamic Variables")
	float ThrottleInput;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Constants")
	float MaxBladeRotationSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Constants")
	float UpForce;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Constants")
	float TurnSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Constants")
	float MaxSpeed;
	
private:
	bool bBladesAtMaxSpeed;
};
