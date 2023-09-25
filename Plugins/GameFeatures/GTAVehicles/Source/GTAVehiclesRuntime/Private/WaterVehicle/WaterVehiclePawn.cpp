// Fill out your copyright notice in the Description page of Project Settings.


#include "WaterVehicle/WaterVehiclePawn.h"

#include "LyraGameplayTags.h"
#include "VehicleExtensionComponent.h"
#include "Camera/LyraCameraComponent.h"
#include "Camera/LyraCameraMode.h"
#include "Input/LyraInputComponent.h"
#include "Kismet/KismetMathLibrary.h"

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

	// FRotator ActorRot = GetActorRotation();
	// FRotator VelocityRotation = UKismetMathLibrary::RLerp(ActorRot, GetVelocity().ToOrientationRotator()
	// 	, .1f, false);
	// SetActorRotation(FRotator(ActorRot.Pitch, VelocityRotation.Yaw, ActorRot.Roll));
}

TSubclassOf<ULyraCameraMode> AWaterVehiclePawn::DetermineCameraMode() const
{
	return CameraMode ? CameraMode : nullptr;
}

void AWaterVehiclePawn::Input_Move(const FInputActionValue& InputActionValue)
{
	FVector Value = InputActionValue.Get<FVector>();
	
	FVector ForwardForce = GetActorForwardVector() * (Value.Y * 250.0f);
	FVector RotationalTorque = FVector(0.0f, 0.f, Value.X * 25.0f);
	
	WaterVehicleMesh->AddForce(ForwardForce, NAME_None, true);
	WaterVehicleMesh->AddTorqueInDegrees(RotationalTorque, TEXT("None"), true);

	FVector NewLinearVelocity = WaterVehicleMesh->GetComponentVelocity().Size() * GetActorForwardVector();
	WaterVehicleMesh->SetPhysicsLinearVelocity(NewLinearVelocity);

	// Clamp the boat's angular velocity (rotation speed)
	FVector AngularVelocity = WaterVehicleMesh->GetPhysicsAngularVelocityInDegrees();
	float MaxAngularVelocity = 50.0f; // Adjust the maximum angular velocity as needed

	if (AngularVelocity.Size() > MaxAngularVelocity)
	{
		AngularVelocity.Normalize();
		AngularVelocity *= MaxAngularVelocity;
		WaterVehicleMesh->SetPhysicsAngularVelocityInDegrees(AngularVelocity);
	}

	// Clamp the boat's component velocity (linear speed)
	FVector Velocity = WaterVehicleMesh->GetComponentVelocity();
	float MaxLinearSpeed = 1200.0f; // Adjust the maximum linear speed as needed

	if (Velocity.Size() > MaxLinearSpeed)
	{
		Velocity.Normalize();
		Velocity *= MaxLinearSpeed;
		WaterVehicleMesh->SetPhysicsLinearVelocity(Velocity);
	}
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
