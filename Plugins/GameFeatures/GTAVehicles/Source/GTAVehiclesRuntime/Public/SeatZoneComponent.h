// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "Interaction/IInteractableTarget.h"
#include "SeatZoneComponent.generated.h"

class ALyraCharacter;

UENUM()
enum class ESeatType : uint8
{
	None,
	Driver,
	Passenger
};

/**
 * 
 */
UCLASS(ClassGroup="Collision", hidecategories=(Object,LOD,Lighting,TextureStreaming), editinlinenew, meta=(DisplayName="Seat Zone Collision", BlueprintSpawnableComponent))
class GTAVEHICLESRUNTIME_API USeatZoneComponent : public UBoxComponent, public IInteractableTarget
{
	GENERATED_BODY()

public:
	USeatZoneComponent();

	virtual void GatherInteractionOptions(const FInteractionQuery& InteractQuery, FInteractionOptionBuilder& OptionBuilder) override;
	virtual void CustomizeInteractionEventData(const FGameplayTag& InteractionEventTag, FGameplayEventData& InOutEventData) override;

	const class ULyraAbilitySet* GetAbilitySet() const;
	const class ULyraInputConfig* GetInputConfig() const;
	const class UInputMappingContext* GetMappingContext() const;
	ESeatType GetSeatType() const;

	UFUNCTION(BlueprintCallable)
	bool IsOccupied() const;
	void SetPawn(ALyraCharacter* Value);

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	void Input_Exit();

	void SetAttachments(bool Value);
	void SetPawnHidden(bool Value);

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="GTA|Vehicle", meta=(AllowPrivateAccess))
	TObjectPtr<class ULyraAbilitySet> AbilitySetToGrant;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="GTA|Vehicle", meta=(AllowPrivateAccess))
	TObjectPtr<class ULyraInputConfig> InputConfig;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="GTA|Vehicle", meta=(AllowPrivateAccess))
	TObjectPtr<class UInputMappingContext> MappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess))
	ESeatType SeatType;

	UPROPERTY()
	TObjectPtr<class AGTAVehicle> Owner_Vehicle;

	UPROPERTY()
	TObjectPtr<class UInputAction> InputAction_Exit;

	UPROPERTY(Replicated)
	TWeakObjectPtr<ALyraCharacter> Pawn;

	UPROPERTY()
	uint32 BindHandle_Exit;
};
