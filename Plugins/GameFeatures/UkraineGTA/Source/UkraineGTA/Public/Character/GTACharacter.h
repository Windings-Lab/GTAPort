// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Character/LyraCharacter.h"
#include "Interfaces/CanSwim.h"
#include "GTACharacter.generated.h"

class ULyraInventoryItemDefinition;
class UGTACombatSet;
/**
 * 
 */
UCLASS()
class UKRAINEGTA_API AGTACharacter : public ALyraCharacter, public ICanSwim
{
	GENERATED_BODY()

public:
	AGTACharacter();

	//~ICanSwim Interface
	UFUNCTION(BlueprintCallable)
		virtual bool IsSwimming() const override;
	UFUNCTION(BlueprintCallable)
		virtual bool IsInWater() const override;
	UFUNCTION(BlueprintCallable)
		virtual bool IsTouchingGroundInWater() const override;
	UFUNCTION(BlueprintCallable)
		virtual bool CanSwimUp() const override;
	//~End of ICanSwim Interface

	UFUNCTION(BlueprintImplementableEvent)
	void K2_OnSetSwimming(bool Value);

	UFUNCTION(BlueprintCallable)
	void AddInitialInventory();

	UFUNCTION(BlueprintImplementableEvent)
	void OnInitialUnarmed();

protected:
	virtual void OnAbilitySystemInitialized() override;
	virtual void OnAbilitySystemUninitialized() override;

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent
		, AActor* OtherActor
		, UPrimitiveComponent* OtherComp
		, int32 OtherBodyIndex, bool bFromSweep
		, const FHitResult& SweepResult);

	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComponent
		, AActor* OtherActor
		, UPrimitiveComponent* OtherComp
		, int32 OtherBodyIndex);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GTA|Character", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UGTAHeroComponent> GTAHeroComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GTA|Character", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UStaminaHandlerComponent> StaminaComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GTA|Character", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UHungerHandlerComponent> HungerComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GTA|Character", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UArmorHandlerComponent> ArmorComponent;

	//~Begin of Water Logic
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GTA|Character", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UWaterLogicComponent> WaterLogicComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GTA|Character", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USphereComponent> WaterHeightChecker;
	//~End of Water Logic

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GTA|Character|Inventory", Meta = (AllowPrivateAccess = "true"))
	TArray<TSubclassOf<ULyraInventoryItemDefinition>> InitialInventoryItems;
};
