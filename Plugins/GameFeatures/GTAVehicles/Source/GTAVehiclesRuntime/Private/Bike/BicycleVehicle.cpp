// Fill out your copyright notice in the Description page of Project Settings.


#include "Bike/BicycleVehicle.h"

#include "VehicleExtensionComponent.h"
#include "Input/LyraInputComponent.h"

void ABicycleVehicle::OnVehicleEnter_Implementation(AActor* CarInstigator, ULyraAbilitySystemComponent* LyraASC)
{
	Super::OnVehicleEnter_Implementation(CarInstigator, LyraASC);
	if(ULyraInputComponent* LyraIC = Cast<ULyraInputComponent>(CarInstigator->InputComponent))
	{
		// if (const UInputAction* IA
		// 	= VehicleExtensionComponent->GetInputConfig()->FindNativeInputActionForTag(LyraGameplayTags::InputTag_Native_Move, false))
		// {
		// 	VehicleExtensionComponent->AddToNativeInputHandle(LyraIC->BindAction(IA, ETriggerEvent::Triggered, this, &ThisClass::Input_Move).GetHandle());
		// }
	}
}

void ABicycleVehicle::OnVehicleExit_Implementation(AActor* CarInstigator, ULyraAbilitySystemComponent* LyraASC)
{
	Super::OnVehicleExit_Implementation(CarInstigator, LyraASC);
}
