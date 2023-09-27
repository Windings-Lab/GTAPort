// Fill out your copyright notice in the Description page of Project Settings.


#include "Aircraft/AircraftPawn.h"

#include "InputActionValue.h"
#include "VehicleExtensionComponent.h"
#include "Camera/LyraCameraComponent.h"
#include "Camera/LyraCameraMode.h"
#include "Input/LyraInputComponent.h"
#include "Kismet/KismetSystemLibrary.h"

namespace GTAAircraft
{
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_Native_Airplane_Thrust, "InputTag.Native.Airplane.Thrust", "Airplane Thrust");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_Native_Airplane_Yaw, "InputTag.Native.Airplane.Yaw", "Airplane Yaw");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_Native_Airplane_Pitch, "InputTag.Native.Airplane.Pitch", "Airplane Pitch");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_Native_Airplane_Roll, "InputTag.Native.Airplane.Roll", "Airplane Roll");
}

// Sets default values
AAircraftPawn::AAircraftPawn()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	
	AircraftMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Fuselage"));
	RootComponent = AircraftMesh;

	CameraComponent = CreateDefaultSubobject<ULyraCameraComponent>(TEXT("CameraCompon"));
	CameraComponent->SetupAttachment(RootComponent);

	VehicleExtensionComponent = CreateDefaultSubobject<UVehicleExtensionComponent>(TEXT("VehicleExtensionComponent"));

	Drag = 0.25f;
	Gravity = 981.f;
	ThrustMultiplier = 2500.f;
	MinThrustToNotFall = 4000.f;
	MaxThrustSpeed = 10000.f;
	
	MaxFlapPitch = 10.f;
	MaxElevatorPitch = 25.f;
	MaxRudderYaw = 45.f;
	MaxAileronPitch = 45.f;

	ThrustSpeed = 0.f;
	CurrentSpeed = 0.f;
	AppliedGravity = 0.f;
	
	TargetYaw = 0.f;
	CurrentYaw = 0.f;
	TargetPitch = 0.f;
	CurrentPitch = 0.f;
	TargetRoll = 0.f;
	CurrentRoll = 0.f;
}

void AAircraftPawn::GatherInteractionOptions(const FInteractionQuery& InteractQuery,
	FInteractionOptionBuilder& OptionBuilder)
{
	OptionBuilder.AddInteractionOption(Option);
}

void AAircraftPawn::CustomizeInteractionEventData(const FGameplayTag& InteractionEventTag,
	FGameplayEventData& InOutEventData)
{
	InOutEventData.OptionalObject = VehicleExtensionComponent;
}

void AAircraftPawn::BeginPlay()
{
	Super::BeginPlay();
}

void AAircraftPawn::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	UpdatePosition(DeltaSeconds);
	PrintVariables();
}

void AAircraftPawn::Input_Thrust(const FInputActionValue& InputActionValue)
{
	float Value = InputActionValue.Get<float>();

	ThrustSpeed = (Value * GetWorld()->GetDeltaSeconds() * ThrustMultiplier) + FMath::Clamp(ThrustSpeed, 0.f, MaxThrustSpeed);
}

void AAircraftPawn::Input_Yaw(const FInputActionValue& InputActionValue)
{
	float Value = InputActionValue.Get<float>();

	UpdateYaw(GetWorld()->GetDeltaSeconds(), Value);
}

void AAircraftPawn::Input_Pitch(const FInputActionValue& InputActionValue)
{
	float Value = InputActionValue.Get<float>();

	UpdatePitch(GetWorld()->GetDeltaSeconds(), Value);
}

void AAircraftPawn::Input_Roll(const FInputActionValue& InputActionValue)
{
	float Value = InputActionValue.Get<float>();

	UpdateRoll(GetWorld()->GetDeltaSeconds(), Value);
}

TSubclassOf<ULyraCameraMode> AAircraftPawn::DetermineCameraMode() const
{
	return CameraMode ? CameraMode : nullptr;
}

void AAircraftPawn::UpdatePosition(float DeltaSeconds)
{
	if(ThrustSpeed < CurrentSpeed)
	{
		CurrentSpeed = FMath::FInterpTo(CurrentSpeed, ThrustSpeed, DeltaSeconds, Drag);
	}
	else
	{
		CurrentSpeed = ThrustSpeed;
	}

	FVector NewPosition = GetActorForwardVector() * (CurrentSpeed * DeltaSeconds);
	AppliedGravity = FMath::GetMappedRangeValueClamped(
		FVector2D(0.f, MinThrustToNotFall)
		, FVector2D(Gravity, 0.f)
		, CurrentSpeed);

	AddActorWorldOffset(FVector(NewPosition.X, NewPosition.Y, NewPosition.Z - AppliedGravity * DeltaSeconds), true);
}

void AAircraftPawn::UpdateYaw(float DeltaSeconds, float Yaw)
{
	TargetYaw = Yaw;
	CurrentYaw = FMath::FInterpTo(CurrentYaw, TargetYaw, DeltaSeconds, 10.f);

	FRotator NewYaw = FRotator(0.f, CurrentYaw * DeltaSeconds * 20.f, 0.f);
	AddActorLocalRotation(NewYaw, true);

	K2_OnUpdateYaw();
}

void AAircraftPawn::UpdatePitch(float DeltaSeconds, float Pitch)
{
	TargetPitch = Pitch;
	CurrentPitch = FMath::FInterpTo(CurrentPitch, TargetPitch, DeltaSeconds, 10.f);

	FRotator NewPitch = FRotator(CurrentPitch * DeltaSeconds * 50.f, 0.f, 0.f);
	AddActorLocalRotation(NewPitch, true);
	
	K2_OnUpdatePitch();
}

void AAircraftPawn::UpdateRoll(float DeltaSeconds, float Roll)
{
	TargetRoll = Roll;
	CurrentRoll = FMath::FInterpTo(CurrentRoll, TargetRoll, DeltaSeconds, 10.f);

	FRotator NewRoll = FRotator(0.f, 0.f, CurrentRoll * DeltaSeconds * 20.f);
	AddActorLocalRotation(NewRoll, true);
	
	K2_OnUpdateRoll();
}

void AAircraftPawn::PrintVariables()
{
	UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("ThrustSpeed: %f"), ThrustSpeed)
		, true, false, FLinearColor::Green, 1.f, TEXT("ThrustSpeed"));

	UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("CurrentSpeed: %f"), CurrentSpeed)
	, true, false, FLinearColor::Green, 1.f, TEXT("CurrentSpeed"));

	UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("AppliedGravity: %f"), AppliedGravity)
	, true, false, FLinearColor::Green, 1.f, TEXT("AppliedGravity"));
}

void AAircraftPawn::OnVehicleEnter_Implementation(AActor* CarInstigator, ULyraAbilitySystemComponent* LyraASC)
{
	if(ULyraInputComponent* LyraIC = Cast<ULyraInputComponent>(CarInstigator->InputComponent))
	{
		const ULyraInputConfig* InputConfig = VehicleExtensionComponent->GetInputConfig();
		if (const UInputAction* IA = InputConfig->FindNativeInputActionForTag(GTAAircraft::InputTag_Native_Airplane_Thrust, false))
		{
			VehicleExtensionComponent->AddToNativeInputHandle(
				LyraIC->BindAction(IA, ETriggerEvent::Triggered, this, &ThisClass::Input_Thrust).GetHandle());
		}

		if (const UInputAction* IA = InputConfig->FindNativeInputActionForTag(GTAAircraft::InputTag_Native_Airplane_Yaw, false))
		{
			VehicleExtensionComponent->AddToNativeInputHandle(
				LyraIC->BindAction(IA, ETriggerEvent::Triggered, this, &ThisClass::Input_Yaw).GetHandle());
		}

		if (const UInputAction* IA = InputConfig->FindNativeInputActionForTag(GTAAircraft::InputTag_Native_Airplane_Pitch, false))
		{
			VehicleExtensionComponent->AddToNativeInputHandle(
				LyraIC->BindAction(IA, ETriggerEvent::Triggered, this, &ThisClass::Input_Pitch).GetHandle());
		}

		if (const UInputAction* IA = InputConfig->FindNativeInputActionForTag(GTAAircraft::InputTag_Native_Airplane_Roll, false))
		{
			VehicleExtensionComponent->AddToNativeInputHandle(
				LyraIC->BindAction(IA, ETriggerEvent::Triggered, this, &ThisClass::Input_Roll).GetHandle());
		}
	}
	CameraComponent->DetermineCameraModeDelegate.BindUObject(this, &ThisClass::DetermineCameraMode);
}

void AAircraftPawn::OnVehicleExit_Implementation(AActor* CarInstigator, ULyraAbilitySystemComponent* LyraASC)
{
	CameraComponent->DetermineCameraModeDelegate.Unbind();
	Controller = nullptr;
}
