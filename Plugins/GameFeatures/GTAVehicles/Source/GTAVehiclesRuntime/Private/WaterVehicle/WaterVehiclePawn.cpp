// Fill out your copyright notice in the Description page of Project Settings.


#include "WaterVehicle/WaterVehiclePawn.h"

#include "LyraGameplayTags.h"
#include "VehicleExtensionComponent.h"
#include "Camera/LyraCameraComponent.h"
#include "Camera/LyraCameraMode.h"
#include "Input/LyraInputComponent.h"

// Sets default values
AWaterVehiclePawn::AWaterVehiclePawn()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	
	WaterVehicleMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WaterVehicleMesh"));
	RootComponent = WaterVehicleMesh;

	CameraComponent = CreateDefaultSubobject<ULyraCameraComponent>(TEXT("CameraCompon"));
	CameraComponent->SetupAttachment(WaterVehicleMesh);

	VehicleExtensionComponent = CreateDefaultSubobject<UVehicleExtensionComponent>(TEXT("VehicleExtensionComponent"));
}

void AWaterVehiclePawn::GatherInteractionOptions(const FInteractionQuery& InteractQuery,
	FInteractionOptionBuilder& OptionBuilder)
{
	OptionBuilder.AddInteractionOption(Option);
}

void AWaterVehiclePawn::CustomizeInteractionEventData(const FGameplayTag& InteractionEventTag,
	FGameplayEventData& InOutEventData)
{
	InOutEventData.OptionalObject = VehicleExtensionComponent;
}

void AWaterVehiclePawn::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

TSubclassOf<ULyraCameraMode> AWaterVehiclePawn::DetermineCameraMode() const
{
	return CameraMode ? CameraMode : nullptr;
}

void AWaterVehiclePawn::Input_Move(const FInputActionValue& InputActionValue)
{
	FVector Value = InputActionValue.Get<FVector>();
	
	WaterVehicleMesh->GetBodyInstance()->AngularDamping = Value.X == 0.f ? 1.f : 0.f;
	WaterVehicleMesh->GetBodyInstance()->LinearDamping  = Value.Y == 0.f ? 1.f : 0.f;

	FVector ForwardVector = GetActorForwardVector();
	ForwardVector.Z = 0.f;
	
	FVector ForwardForce = ForwardVector * (Value.Y * 1000.0f);
	FVector RotationalTorque = FVector(0.0f, 0.f, Value.X * 50.0f);
	
	WaterVehicleMesh->AddForce(ForwardForce, NAME_None, true);
	WaterVehicleMesh->AddTorqueInDegrees(RotationalTorque, TEXT("None"), true);
}

void AWaterVehiclePawn::OnVehicleEnter_Implementation(AActor* CarInstigator, ULyraAbilitySystemComponent* LyraASC)
{
	if(ULyraInputComponent* LyraIC = Cast<ULyraInputComponent>(CarInstigator->InputComponent))
	{
		if (const UInputAction* IA
			= VehicleExtensionComponent->GetInputConfig()->FindNativeInputActionForTag(LyraGameplayTags::InputTag_Native_Move, false))
		{
			VehicleExtensionComponent->AddToNativeInputHandle(LyraIC->BindAction(IA, ETriggerEvent::Triggered, this, &ThisClass::Input_Move).GetHandle());
		}
	}
	CameraComponent->DetermineCameraModeDelegate.BindUObject(this, &ThisClass::DetermineCameraMode);
}

void AWaterVehiclePawn::OnVehicleExit_Implementation(AActor* CarInstigator, ULyraAbilitySystemComponent* LyraASC)
{
	CameraComponent->DetermineCameraModeDelegate.Unbind();
	Controller = nullptr;
}
