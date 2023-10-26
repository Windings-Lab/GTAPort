// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "Interfaces/CanSwim.h"
#include "WaterLogicComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UKRAINEGTA_API UWaterLogicComponent : public UActorComponent, public ICanSwim
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UWaterLogicComponent();

	void SetAffectedByWater(bool Value);
	void SetSwimming(bool Value);
	void SetUnderWater(bool Value);

	virtual bool IsSwimming() const override;
	virtual bool AffectedByWater() const override;
	virtual bool IsTouchingGroundInWater() const override;
	virtual bool IsUnderWater() const override;

protected:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
private:
	bool bSwimming;
	bool bWaterAffection;
	bool bTouchingGround;
	bool bUnderWater;
};
