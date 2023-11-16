// Fill out your copyright notice in the Description page of Project Settings.


#include "GTAVehicle.h"

#include "GTAVehicleGameplayTags.h"
#include "SeatZoneComponent.h"
#include "VehicleExtensionComponent.h"
#include "AbilitySystem/LyraAbilitySystemComponent.h"
#include "Character/LyraCharacter.h"
#include "Input/LyraInputComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"

AGTAVehicle::AGTAVehicle(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AIControllerClass = nullptr;
	SetReplicates(true);
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	GetMesh()->BodyInstance.bSimulatePhysics = true;
	
	VehicleExtensionComponent = CreateDefaultSubobject<UVehicleExtensionComponent>(TEXT("VehicleExtensionComponent"));
}

UVehicleExtensionComponent* AGTAVehicle::GetVehicleExtensionComponent() const
{
	return VehicleExtensionComponent;
}

void AGTAVehicle::SetSeatController(ESeatType SeatType, AController* InController)
{
	if(!HasAuthority()) return;
	
	if(SeatType == ESeatType::Driver)
	{
		Controller = InController;
		bDriverEntered = Controller != nullptr;
	}
}

void AGTAVehicle::OnVehicleEnter_Implementation(ALyraCharacter* PawnInstigator, USeatZoneComponent* SeatZone)
{
	SetSeatController(SeatZone->GetSeatType(), PawnInstigator->Controller);
	
	ULyraInputComponent* LyraIC = Cast<ULyraInputComponent>(PawnInstigator->InputComponent);
	if(!LyraIC) return;

	const ULyraInputConfig* InputConfig = SeatZone->GetInputConfig();
	if(!InputConfig) return;
	
	if (const UInputAction* IA = InputConfig->FindNativeInputActionForTag(GTAVehicleGameplayTags::InputTag_Native_Vehicle_Move, false))
	{
		VehicleExtensionComponent->AddToNativeInputHandle(
			LyraIC->BindAction(IA, ETriggerEvent::Triggered, this, &ThisClass::Input_Move).GetHandle());
	}

	if (const UInputAction* IA = InputConfig->FindNativeInputActionForTag(GTAVehicleGameplayTags::InputTag_Native_Vehicle_Yaw, false))
	{
		VehicleExtensionComponent->AddToNativeInputHandle(
			LyraIC->BindAction(IA, ETriggerEvent::Triggered, this, &ThisClass::Input_Yaw).GetHandle());
	}

	if (const UInputAction* IA = InputConfig->FindNativeInputActionForTag(GTAVehicleGameplayTags::InputTag_Native_Vehicle_Pitch, false))
	{
		VehicleExtensionComponent->AddToNativeInputHandle(
			LyraIC->BindAction(IA, ETriggerEvent::Triggered, this, &ThisClass::Input_Pitch).GetHandle());
	}

	if (const UInputAction* IA = InputConfig->FindNativeInputActionForTag(GTAVehicleGameplayTags::InputTag_Native_Vehicle_Roll, false))
	{
		VehicleExtensionComponent->AddToNativeInputHandle(
			LyraIC->BindAction(IA, ETriggerEvent::Triggered, this, &ThisClass::Input_Roll).GetHandle());
	}
}
void AGTAVehicle::OnVehicleExit_Implementation(ALyraCharacter* PawnInstigator, USeatZoneComponent* SeatZone)
{
	SetSeatController(SeatZone->GetSeatType(), nullptr);
}

bool AGTAVehicle::PrintVariables() const
{
	if(!Controller) return false;
	if(Controller->IsLocalPlayerController())
	{
		UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("MoveInput: %s"), *MoveInput.ToString())
		, true, false, FLinearColor::Green, 1.f, TEXT("Vehicle_Move"));
		
		UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("YawInput: %f"), YawInput)
		, true, false, FLinearColor::Green, 1.f, TEXT("Vehicle_Yaw"));
		
		UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("PitchInput: %f"), PitchInput)
		, true, false, FLinearColor::Green, 1.f, TEXT("Vehicle_Pitch"));
		
		UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("RollInput: %f"), RollInput)
		, true, false, FLinearColor::Green, 1.f, TEXT("Vehicle_Roll"));
		
		return true;
	}

	return false;
}

bool AGTAVehicle::InputDetected() const
{
	return !MoveInput.IsZero() || RollInput || PitchInput || YawInput;
}

void AGTAVehicle::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams SharedParams;

	DOREPLIFETIME_CONDITION(ThisClass, MoveInput, COND_None)
	DOREPLIFETIME_CONDITION(ThisClass, YawInput, COND_None)
	DOREPLIFETIME_CONDITION(ThisClass, PitchInput, COND_None)
	DOREPLIFETIME_CONDITION(ThisClass, RollInput, COND_None)

	DOREPLIFETIME(ThisClass, bDriverEntered)
}

void AGTAVehicle::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// ReSharper disable once CppExpressionWithoutSideEffects
	PrintVariables();
}

void AGTAVehicle::BeginPlay()
{
	Super::BeginPlay();
	SetSeatController(ESeatType::Driver, nullptr);
}

// **********************
// ~ Input ~
// **********************
void AGTAVehicle::Input_Move(const FInputActionValue& InputActionValue)
{
	const FVector InputValue = InputActionValue.Get<FVector>();
	MoveInput = InputValue;
	Server_Input_Move(InputValue);
}
void AGTAVehicle::Server_Input_Move_Implementation(FVector InputValue)
{
	MoveInput = InputValue;
}

void AGTAVehicle::Input_Pitch(const FInputActionValue& InputActionValue)
{
	const float InputValue = InputActionValue.Get<float>();
	PitchInput = InputValue;
	Server_Input_Pitch(InputValue);
}
void AGTAVehicle::Server_Input_Pitch_Implementation(float InputValue)
{
	PitchInput = InputValue;
}

void AGTAVehicle::Input_Roll(const FInputActionValue& InputActionValue)
{
	const float InputValue = InputActionValue.Get<float>();
	RollInput = InputValue;
	Server_Input_Roll(InputValue);
}
void AGTAVehicle::Server_Input_Roll_Implementation(float InputValue)
{
	RollInput = InputValue;
}

void AGTAVehicle::Input_Yaw(const FInputActionValue& InputActionValue)
{
	const float InputValue = InputActionValue.Get<float>();
	YawInput = InputValue;
	Server_Input_Yaw(InputValue);
}
void AGTAVehicle::Server_Input_Yaw_Implementation(float InputValue)
{
	YawInput = InputValue;
}
// **********************
// ~ Input ~
// **********************
