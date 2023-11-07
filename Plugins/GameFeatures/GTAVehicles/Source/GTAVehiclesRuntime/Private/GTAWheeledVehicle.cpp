// Fill out your copyright notice in the Description page of Project Settings.


#include "GTAWheeledVehicle.h"

#include "ChaosVehicleMovementComponent.h"
#include "LyraGameplayTags.h"
#include "VehicleExtensionComponent.h"
#include "AbilitySystem/LyraAbilitySystemComponent.h"
#include "Camera/LyraCameraComponent.h"
#include "Camera/LyraCameraMode.h"
#include "Input/LyraInputComponent.h"

AGTAWheeledVehicle::AGTAWheeledVehicle(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	VehicleExtensionComponent = CreateDefaultSubobject<UVehicleExtensionComponent>(TEXT("VehicleExtensionComponent"));
	VehicleExtensionComponent->CameraComponent = CreateDefaultSubobject<ULyraCameraComponent>(TEXT("CameraComponent"));
	VehicleExtensionComponent->CameraComponent->SetupAttachment(RootComponent);
}

void AGTAWheeledVehicle::OnVehicleEnter_Implementation(AActor* CarInstigator, ULyraAbilitySystemComponent* LyraASC)
{
	if(ULyraInputComponent* LyraIC = Cast<ULyraInputComponent>(CarInstigator->InputComponent))
	{
		if (const UInputAction* IA
			= VehicleExtensionComponent->GetInputConfig()->FindNativeInputActionForTag(LyraGameplayTags::InputTag_Native_Move, false))
		{
			VehicleExtensionComponent->AddToNativeInputHandle(LyraIC->BindAction(IA, ETriggerEvent::Triggered, this, &ThisClass::Input_Move).GetHandle());
		}
	}
}

void AGTAWheeledVehicle::GatherInteractionOptions(const FInteractionQuery& InteractQuery,
                                                  FInteractionOptionBuilder& OptionBuilder)
{
	OptionBuilder.AddInteractionOption(VehicleExtensionComponent->GetInteractionOption());
}

void AGTAWheeledVehicle::CustomizeInteractionEventData(const FGameplayTag& InteractionEventTag,
	FGameplayEventData& InOutEventData)
{
	InOutEventData.OptionalObject = VehicleExtensionComponent;
}

void AGTAWheeledVehicle::OnVehicleExit_Implementation(AActor* CarInstigator, ULyraAbilitySystemComponent* LyraASC)
{
}

void AGTAWheeledVehicle::Input_Move(const FInputActionValue& InputActionValue)
{
	FVector Value = InputActionValue.Get<FVector>();

	GetVehicleMovement()->SetThrottleInput(Value.Y);
	GetVehicleMovement()->SetBrakeInput(Value.Z);
	GetVehicleMovement()->SetSteeringInput(Value.X);
}
