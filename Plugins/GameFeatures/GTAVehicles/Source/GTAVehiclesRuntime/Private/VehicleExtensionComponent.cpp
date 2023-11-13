// Fill out your copyright notice in the Description page of Project Settings.


#include "VehicleExtensionComponent.h"

#include "AbilitySystem/LyraAbilitySystemComponent.h"
#include "Camera/LyraCameraComponent.h"
#include "Camera/LyraCameraMode.h"
#include "Character/LyraHeroComponent.h"
#include "Net/UnrealNetwork.h"

UVehicleExtensionComponent::UVehicleExtensionComponent()
{
	SetIsReplicatedByDefault(true);
}

void UVehicleExtensionComponent::BeginPlay()
{
	Super::BeginPlay();

	check(AbilitySetToGrant && "AbilitySetToGrant not defined");
	check(InputConfig && "InputConfig not defined");
	check(MappingContext && "MappingContext not defined");
}

void UVehicleExtensionComponent::SetAbilities(APawn* Pawn, ULyraAbilitySystemComponent* LyraASC, bool Value)
{
	auto* HeroComponent = ULyraHeroComponent::FindHeroComponent(Pawn);
	if(!HeroComponent) return;
	
	if(Value)
	{
		if(AbilitySetToGrant)
		{
			AbilitySetToGrant->GiveToAbilitySystem(LyraASC, &LoadedAbilitySetHandles);
		}

		const auto AdditionalBindHandles = HeroComponent->AddAdditionalInputConfig(InputConfig);
		LoadedBindHandles.Append(AdditionalBindHandles);
	}
	else
	{
		HeroComponent->RemoveAdditionalInputConfig(LoadedBindHandles);
		LoadedAbilitySetHandles.TakeFromAbilitySystem(LyraASC);
	}
}

void UVehicleExtensionComponent::SetInputs(APawn* Pawn, ULyraAbilitySystemComponent* LyraASC, bool Value)
{
	auto* HeroComponent = ULyraHeroComponent::FindHeroComponent(Pawn);
	if(!HeroComponent) return;
	
	APlayerController* PawnController = Pawn->GetController<APlayerController>();
	if(!PawnController) return;
	
	PawnController->SetViewTargetWithBlend(Value ? GetOwner() : Pawn);
	if(Value)
	{
		if(CameraComponent)
		{
			CameraComponent->DetermineCameraModeDelegate.BindUObject(this, &ThisClass::DetermineCameraMode);
		}

		HeroComponent->AddAdditionalMappingContext(MappingContext, 1);
	}
	else
	{
		if(CameraComponent)
		{
			CameraComponent->DetermineCameraModeDelegate.Unbind();
		}
		
		HeroComponent->RemoveAdditionalMappingContext(MappingContext);
	}
}

void UVehicleExtensionComponent::SetAttachments(APawn* Pawn, ULyraAbilitySystemComponent* LyraASC, bool Value)
{
	SetPawnHidden(Pawn, Value);
	Pawn->SetActorEnableCollision(!Value);
	if(Value)
	{
		EnteredRelativePosition = GetOwner()->GetTransform().InverseTransformPosition(Pawn->GetActorLocation());
	}
	else
	{
		FVector WorldPosition = GetOwner()->GetTransform().TransformPosition(EnteredRelativePosition);
		if(Pawn->HasAuthority())
		{
			Pawn->TeleportTo(WorldPosition, FRotator(0.f, 0.f, 0.f));
		}
	}
}

void UVehicleExtensionComponent::OnVehicleEnter_Implementation(AActor* PawnInstigator, ULyraAbilitySystemComponent* LyraASC)
{
	auto Test = PawnInstigator->GetNetMode();
	auto* VehiclePawn = Cast<APawn>(GetOwner());
	if(bDriverEntered) return;
	
	auto* Pawn = Cast<APawn>(PawnInstigator);

	VehiclePawn->Controller = Pawn->Controller;
	bDriverEntered = true;
	Execute_OnVehicleEnter(VehiclePawn, PawnInstigator, LyraASC);
	
	SetAbilities(Pawn, LyraASC, true);
	SetInputs(Pawn, LyraASC, true);
	SetAttachments(Pawn, LyraASC, true);
}

void UVehicleExtensionComponent::OnVehicleExit_Implementation(AActor* PawnInstigator, ULyraAbilitySystemComponent* LyraASC)
{
	auto* VehiclePawn = Cast<APawn>(GetOwner());
	auto* Pawn = Cast<APawn>(PawnInstigator);

	Execute_OnVehicleExit(VehiclePawn, PawnInstigator, LyraASC);
	SetAbilities(Pawn, LyraASC, false);
	SetInputs(Pawn, LyraASC, false);
	SetAttachments(Pawn, LyraASC, false);

	VehiclePawn->Controller = nullptr;
	bDriverEntered = false;
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

const ULyraInputConfig* UVehicleExtensionComponent::GetInputConfig() const
{
	return InputConfig;
}

FInteractionOption& UVehicleExtensionComponent::GetInteractionOption()
{
	return Option;
}

bool UVehicleExtensionComponent::WithDriver() const
{
	return bDriverEntered;
}

void UVehicleExtensionComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ThisClass, bDriverEntered, COND_SkipOwner)
}

TSubclassOf<ULyraCameraMode> UVehicleExtensionComponent::DetermineCameraMode() const
{
	return CameraMode ? CameraMode : nullptr;
}
