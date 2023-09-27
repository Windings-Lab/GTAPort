// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NativeGameplayTags.h"
#include "GameFramework/Pawn.h"
#include "Interaction/IInteractableTarget.h"
#include "Interfaces/Vehicle.h"
#include "AircraftPawn.generated.h"

struct FInputActionValue;
class ULyraCameraComponent;
class UVehicleExtensionComponent;
class ULyraCameraMode;

namespace GTAAircraft
{
	GTAVEHICLESRUNTIME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Native_Airplane_Thrust);
	GTAVEHICLESRUNTIME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Native_Airplane_Yaw);
	GTAVEHICLESRUNTIME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Native_Airplane_Pitch);
	GTAVEHICLESRUNTIME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Native_Airplane_Roll);
}

UCLASS()
class GTAVEHICLESRUNTIME_API AAircraftPawn : public APawn, public IInteractableTarget, public IVehicle
{
	GENERATED_BODY()

public:
	AAircraftPawn();

	virtual void GatherInteractionOptions(const FInteractionQuery& InteractQuery, FInteractionOptionBuilder& OptionBuilder) override;
	virtual void CustomizeInteractionEventData(const FGameplayTag& InteractionEventTag, FGameplayEventData& InOutEventData) override;

	UFUNCTION(BlueprintImplementableEvent)
	void K2_OnUpdateYaw();

	UFUNCTION(BlueprintImplementableEvent)
	void K2_OnUpdatePitch();
	
	UFUNCTION(BlueprintImplementableEvent)
	void K2_OnUpdateRoll();

protected:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnVehicleEnter(AActor* CarInstigator, ULyraAbilitySystemComponent* LyraASC);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnVehicleExit(AActor* CarInstigator, ULyraAbilitySystemComponent* LyraASC);

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

private:
	void Input_Thrust(const FInputActionValue& InputActionValue);
	void Input_Yaw(const FInputActionValue& InputActionValue);
	void Input_Pitch(const FInputActionValue& InputActionValue);
	void Input_Roll(const FInputActionValue& InputActionValue);
	
	TSubclassOf<ULyraCameraMode> DetermineCameraMode() const;

	void UpdatePosition(float DeltaSeconds);
	
	void UpdateYaw(float DeltaSeconds, float Yaw);
	void UpdatePitch(float DeltaSeconds, float Pitch);
	void UpdateRoll(float DeltaSeconds, float Roll);
	
	void PrintVariables();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> AircraftMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<ULyraCameraComponent> CameraComponent;

	UPROPERTY(EditDefaultsOnly, Category="VehicleSettings")
	FInteractionOption Option;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lyra|Camera")
	TSubclassOf<ULyraCameraMode> CameraMode;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UVehicleExtensionComponent> VehicleExtensionComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Constants")
	float MaxThrustSpeed;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Constants")
	float MinThrustToNotFall;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Constants")
	float ThrustMultiplier;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Constants")
	float Gravity;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Constants")
	float Drag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Constants|Control Surfaces")
	float MaxFlapPitch;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Constants|Control Surfaces")
	float MaxElevatorPitch;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Constants|Control Surfaces")
	float MaxRudderYaw;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Constants|Control Surfaces")
	float MaxAileronPitch;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dynamic Variables")
	float ThrustSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dynamic Variables")
	float CurrentSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dynamic Variables")
	float AppliedGravity;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dynamic Variables|Rotation")
	float TargetYaw;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dynamic Variables|Rotation")
	float CurrentYaw;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dynamic Variables|Rotation")
	float TargetPitch;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dynamic Variables|Rotation")
	float CurrentPitch;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dynamic Variables|Rotation")
	float TargetRoll;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dynamic Variables|Rotation")
	float CurrentRoll;
};
