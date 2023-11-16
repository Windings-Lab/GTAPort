// Fill out your copyright notice in the Description page of Project Settings.


#include "VehicleExtensionComponent.h"

#include "GTAVehicle.h"
#include "SeatZoneComponent.h"
#include "Character/LyraCharacter.h"
#include "Character/LyraHeroComponent.h"

UVehicleExtensionComponent::UVehicleExtensionComponent()
{
	SetIsReplicatedByDefault(true);
}

void UVehicleExtensionComponent::SetAbilities(ALyraCharacter* Pawn, const USeatZoneComponent* SeatZone, bool Value)
{
	auto* HeroComponent = ULyraHeroComponent::FindHeroComponent(Pawn);
	if(!HeroComponent) return;

	auto* LyraASC = Pawn->GetLyraAbilitySystemComponent();
	if(!LyraASC) return;
	
	if(Value)
	{
		auto* AbilitySet = SeatZone->GetAbilitySet();
		auto* InputConfig = SeatZone->GetInputConfig();
		
		if(AbilitySet)
		{
			AbilitySet->GiveToAbilitySystem(LyraASC, &LoadedAbilitySetHandles);
		}

		if(InputConfig)
		{
			const auto AdditionalBindHandles = HeroComponent->AddAdditionalInputConfig(InputConfig);
			LoadedBindHandles.Append(AdditionalBindHandles);
		}
	}
	else
	{
		HeroComponent->RemoveAdditionalInputConfig(LoadedBindHandles);
		LoadedAbilitySetHandles.TakeFromAbilitySystem(LyraASC);
	}
}

void UVehicleExtensionComponent::SetInputs(ALyraCharacter* Pawn, const USeatZoneComponent* SeatZone, bool Value)
{
	auto* HeroComponent = ULyraHeroComponent::FindHeroComponent(Pawn);
	if(!HeroComponent) return;

	auto* MappingContext = SeatZone->GetMappingContext();
	
	if(Value)
	{
		if(MappingContext)
		{
			HeroComponent->AddAdditionalMappingContext(MappingContext, 1);
		}
	}
	else
	{
		
		HeroComponent->RemoveAdditionalMappingContext(MappingContext);
	}
}

void UVehicleExtensionComponent::OnVehicleEnter_Implementation(ALyraCharacter* PawnInstigator, USeatZoneComponent* SeatZone)
{
	if(SeatZone->IsOccupied()) return;
	SeatZone->SetPawn(PawnInstigator);
	
	auto* VehiclePawn = Cast<AGTAVehicle>(GetOwner());
	
	Execute_OnVehicleEnter(VehiclePawn, PawnInstigator, SeatZone);
	
	SetAbilities(PawnInstigator, SeatZone, true);
	SetInputs(PawnInstigator, SeatZone, true);
}

void UVehicleExtensionComponent::OnVehicleExit_Implementation(ALyraCharacter* PawnInstigator, USeatZoneComponent* SeatZone)
{
	SeatZone->SetPawn(nullptr);
	auto* VehiclePawn = Cast<AGTAVehicle>(GetOwner());

	Execute_OnVehicleExit(VehiclePawn, PawnInstigator, SeatZone);
	SetAbilities(PawnInstigator, SeatZone, false);
	SetInputs(PawnInstigator, SeatZone, false);
}

void UVehicleExtensionComponent::SetPawnHidden(APawn* Pawn, bool Value)
{
	if(!Pawn) return;

	TArray<AActor*> AttachedActors;
	Pawn->GetAttachedActors(AttachedActors, false);
	for (auto* AttachedActor : AttachedActors)
	{
		AttachedActor->SetActorHiddenInGame(Value);
	}
	Pawn->SetActorHiddenInGame(Value);
}

void UVehicleExtensionComponent::AddToNativeInputHandle(int32 Handle)
{
	LoadedBindHandles.Add(Handle);
}

FInteractionOption& UVehicleExtensionComponent::GetInteractionOption()
{
	return Option;
}
