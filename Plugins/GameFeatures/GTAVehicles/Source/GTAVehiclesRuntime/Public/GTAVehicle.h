// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "WheeledVehiclePawn.h"
#include "Interfaces/Vehicle.h"
#include "GTAVehicle.generated.h"

enum class ESeatType : uint8;

USTRUCT()
struct FSeatReplication
{
	GENERATED_BODY()

	UPROPERTY()
	ESeatType SeatType;

	UPROPERTY()
	TWeakObjectPtr<AController> Controller;
};

/**
 * 
 */
UCLASS(Abstract, BlueprintType)
class GTAVEHICLESRUNTIME_API AGTAVehicle : public AWheeledVehiclePawn, public IVehicle
{
	GENERATED_BODY()

public:
	AGTAVehicle(const FObjectInitializer& ObjectInitializer);

	class UVehicleExtensionComponent* GetVehicleExtensionComponent() const;

protected:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnVehicleEnter(ALyraCharacter* PawnInstigator, USeatZoneComponent* SeatZone);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnVehicleExit(ALyraCharacter* PawnInstigator, USeatZoneComponent* SeatZone);
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void Tick(float DeltaSeconds) override;
	virtual void BeginPlay() override;

	virtual bool PrintVariables() const;
	bool InputDetected() const;
	
	void SetSeatController(ESeatType SeatType, AController* InController);

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

	UPROPERTY(Replicated)
	bool bDriverEntered;

};
