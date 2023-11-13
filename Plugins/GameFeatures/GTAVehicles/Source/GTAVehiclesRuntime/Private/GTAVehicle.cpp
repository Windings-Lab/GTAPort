// Fill out your copyright notice in the Description page of Project Settings.


#include "GTAVehicle.h"

#include "GTAVehicleGameplayTags.h"
#include "VehicleExtensionComponent.h"
#include "AbilitySystem/LyraAbilitySystemComponent.h"
#include "Camera/LyraCameraComponent.h"
#include "Character/LyraCharacter.h"
#include "Input/LyraInputComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"

UE_DEFINE_GAMEPLAY_TAG_STATIC(Ability_Behavior_VehicleControl, "Ability.Behavior.VehicleControl");

AGTAVehicle::AGTAVehicle(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AIControllerClass = nullptr;
	SetReplicates(true);
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	GetMesh()->BodyInstance.bSimulatePhysics = true;
	
	VehicleExtensionComponent = CreateDefaultSubobject<UVehicleExtensionComponent>(TEXT("VehicleExtensionComponent"));
	VehicleExtensionComponent->CameraComponent = CreateDefaultSubobject<ULyraCameraComponent>(TEXT("CameraComponent"));
	VehicleExtensionComponent->CameraComponent->SetupAttachment(RootComponent);

	static const auto VehicleExitInputPath = L"/Script/EnhancedInput.InputAction'/GTAVehicles/Input/Actions/IA_VehicleExit.IA_VehicleExit'";
	static const ConstructorHelpers::FObjectFinder<UInputAction> VehicleExitInputObj(VehicleExitInputPath);
	VehicleExitInput = VehicleExitInputObj.Object;
}
void AGTAVehicle::GatherInteractionOptions(const FInteractionQuery& InteractQuery,
                                                  FInteractionOptionBuilder& OptionBuilder)
{
	OptionBuilder.AddInteractionOption(VehicleExtensionComponent->GetInteractionOption());
}

void AGTAVehicle::CustomizeInteractionEventData(const FGameplayTag& InteractionEventTag,
	FGameplayEventData& InOutEventData)
{
	InOutEventData.OptionalObject = VehicleExtensionComponent;
}

void AGTAVehicle::OnVehicleEnter_Implementation(AActor* PawnInstigator, ULyraAbilitySystemComponent* LyraASC)
{
	ULyraInputComponent* LyraIC = Cast<ULyraInputComponent>(PawnInstigator->InputComponent);
	if(!LyraIC) return;

	const ULyraInputConfig* InputConfig = VehicleExtensionComponent->GetInputConfig();
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

	VehicleExtensionComponent->AddToNativeInputHandle(
		LyraIC->BindAction(VehicleExitInput, ETriggerEvent::Triggered, this, &ThisClass::Input_Exit).GetHandle());
}
void AGTAVehicle::OnVehicleExit_Implementation(AActor* CarInstigator, ULyraAbilitySystemComponent* LyraASC)
{
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

	DOREPLIFETIME_CONDITION(ThisClass, MoveInput, COND_None)
	DOREPLIFETIME_CONDITION(ThisClass, YawInput, COND_None)
	DOREPLIFETIME_CONDITION(ThisClass, PitchInput, COND_None)
	DOREPLIFETIME_CONDITION(ThisClass, RollInput, COND_None)
}

void AGTAVehicle::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// ReSharper disable once CppExpressionWithoutSideEffects
	PrintVariables();
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

void AGTAVehicle::Input_Exit()
{
	auto* Pawn = Controller->GetPawn<ALyraCharacter>();
	auto* LyraASC = Pawn->GetLyraAbilitySystemComponent();
	FGameplayTagContainer GameplayTagContainer;
	GameplayTagContainer.AddTag(Ability_Behavior_VehicleControl);
	LyraASC->CancelAbilities(&GameplayTagContainer);
}
// **********************
// ~ Input ~
// **********************