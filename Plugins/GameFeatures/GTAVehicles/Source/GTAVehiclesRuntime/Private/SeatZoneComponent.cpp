// Fill out your copyright notice in the Description page of Project Settings.


#include "SeatZoneComponent.h"

#include "EnhancedInputComponent.h"
#include "GTAVehicle.h"
#include "GTAVehicleGameplayTags.h"
#include "VehicleExtensionComponent.h"
#include "AbilitySystem/LyraAbilitySystemComponent.h"
#include "Character/LyraCharacter.h"
#include "Input/LyraInputComponent.h"
#include "Net/UnrealNetwork.h"

USeatZoneComponent::USeatZoneComponent() : SeatType(ESeatType::None), BindHandle_Exit(-1)
{
	SetIsReplicatedByDefault(true);
	
	static const auto VehicleExitInputPath =
		L"/Script/EnhancedInput.InputAction'/Game/Input/Actions/Cook/IA_VehicleExit.IA_VehicleExit'";
	static const ConstructorHelpers::FObjectFinder<UInputAction> VehicleExitInputObj(VehicleExitInputPath);
	InputAction_Exit = VehicleExitInputObj.Object;
}

void USeatZoneComponent::GatherInteractionOptions(const FInteractionQuery& InteractQuery,
                                                  FInteractionOptionBuilder& OptionBuilder)
{
	if(!IsOccupied())
	{
		OptionBuilder.AddInteractionOption(Owner_Vehicle->GetVehicleExtensionComponent()->GetInteractionOption());
	}
}

void USeatZoneComponent::CustomizeInteractionEventData(const FGameplayTag& InteractionEventTag,
	FGameplayEventData& InOutEventData)
{
	InOutEventData.OptionalObject = Owner_Vehicle->GetVehicleExtensionComponent();
	InOutEventData.OptionalObject2 = this;
}

const ULyraAbilitySet* USeatZoneComponent::GetAbilitySet() const
{
	return AbilitySetToGrant;
}

const ULyraInputConfig* USeatZoneComponent::GetInputConfig() const
{
	return InputConfig;
}

const UInputMappingContext* USeatZoneComponent::GetMappingContext() const
{
	return MappingContext;
}

ESeatType USeatZoneComponent::GetSeatType() const
{
	return SeatType;
}

bool USeatZoneComponent::IsOccupied() const
{
	return !Pawn.IsExplicitlyNull();
}

void USeatZoneComponent::SetPawn(ALyraCharacter* Value)
{
	if(Value)
	{
		if(ULyraInputComponent* LyraIC = Cast<ULyraInputComponent>(Value->InputComponent))
		{
			BindHandle_Exit = LyraIC->BindAction(InputAction_Exit, ETriggerEvent::Triggered, this, &ThisClass::Input_Exit).GetHandle();
		}

		Pawn = Value;
		SetAttachments(true);
	}
	else if(IsOccupied())
	{
		if(Pawn.Get())
		{
			if(ULyraInputComponent* LyraIC = Cast<ULyraInputComponent>(Pawn->InputComponent))
			{
				TArray BindHandleArray = {BindHandle_Exit};
				LyraIC->RemoveBinds(BindHandleArray);
				BindHandle_Exit = -1;
			}
		}

		SetAttachments(false);
		Pawn = Value;
	}
}

void USeatZoneComponent::BeginPlay()
{
	Super::BeginPlay();

	AGTAVehicle* Vehicle = Cast<AGTAVehicle>(GetOwner());
	check(Vehicle)
	Owner_Vehicle = Vehicle;
}

void USeatZoneComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, Pawn)
}

void USeatZoneComponent::Input_Exit()
{
	auto* LyraASC = Pawn->GetAbilitySystemComponent();
	FGameplayTagContainer GameplayTagContainer;
	GameplayTagContainer.AddTag(GTAVehicleGameplayTags::Ability_Behavior_VehicleControl);
	LyraASC->CancelAbilities(&GameplayTagContainer);
}

void USeatZoneComponent::SetAttachments(bool Value)
{
	if(Pawn.Get() == nullptr) return;
	const auto* VehicleOwner = Cast<AGTAVehicle>(GetOwner());

	SetPawnHidden(Value);
	Pawn->SetActorEnableCollision(!Value);
	if(Value)
	{
		FAttachmentTransformRules AttachmentTransformRules = FAttachmentTransformRules(
			EAttachmentRule::SnapToTarget
			, EAttachmentRule::KeepWorld
			, EAttachmentRule::KeepWorld
			, false);

		Pawn->GetRootComponent()->SetAbsolute(false, true, true);
		Pawn->AttachToComponent(VehicleOwner->GetMesh(), AttachmentTransformRules, GetAttachSocketName());
		Pawn->SetReplicateMovement(false);
		Pawn->SetOptionalCameraTarget(Owner_Vehicle.Get());
	}
	else
	{
		FDetachmentTransformRules DetachmentTransformRules = FDetachmentTransformRules(
			EDetachmentRule::KeepWorld
			, EDetachmentRule::KeepWorld
			, EDetachmentRule::KeepWorld
			, false);
		Pawn->DetachFromActor(DetachmentTransformRules);
		Pawn->GetRootComponent()->SetAbsolute(false, false, false);
		Pawn->SetReplicateMovement(true);
		Pawn->SetOptionalCameraTarget(nullptr);
	}
}

void USeatZoneComponent::SetPawnHidden(bool Value)
{
	TArray<AActor*> AttachedActors;
	Pawn->GetAttachedActors(AttachedActors, false);
	for (auto* AttachedActor : AttachedActors)
	{
		AttachedActor->SetActorHiddenInGame(Value);
	}
	Pawn->SetActorHiddenInGame(Value);
}
