// Fill out your copyright notice in the Description page of Project Settings.


#include "VehicleExtensionComponent.h"

#include "EnhancedInputSubsystems.h"
#include "AbilitySystem/LyraAbilitySystemComponent.h"
#include "Camera/LyraCameraComponent.h"
#include "Camera/LyraCameraMode.h"
#include "Character/LyraPawnExtensionComponent.h"
#include "Input/LyraInputComponent.h"
#include "Player/LyraLocalPlayer.h"

UVehicleExtensionComponent::UVehicleExtensionComponent() : AbilitySetToGrant(nullptr), InputConfig(nullptr)
{
}

void UVehicleExtensionComponent::OnVehicleEnter_Implementation(AActor* CarInstigator, ULyraAbilitySystemComponent* LyraASC)
{
	EnteredPawn = Cast<APawn>(CarInstigator);
	Execute_OnVehicleEnter(GetOwner(), CarInstigator, LyraASC);
	
	if(!AbilitySetToGrant) return;
	AbilitySetToGrant->GiveToAbilitySystem(LyraASC, &LoadedAbilitySetHandles);

	if(ULyraInputComponent* LyraIC = Cast<ULyraInputComponent>(CarInstigator->InputComponent))
	{
		LyraIC->BindAbilityActions(InputConfig
			, this, &ThisClass::Input_AbilityInputTagPressed, &ThisClass::Input_AbilityInputTagReleased, /*out*/ LoadedBindHandles);
	}
	
	APlayerController* PawnController = EnteredPawn->GetController<APlayerController>();
	if(PawnController)
	{
		PawnController->SetViewTargetWithBlend(GetOwner());
		const ULyraLocalPlayer* LP = Cast<ULyraLocalPlayer>(PawnController->GetLocalPlayer());
		if(LP)
		{
			UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
			if(Subsystem)
			{
				FModifyContextOptions Options;
				Options.bForceImmediately = false;
				Options.bNotifyUserSettings = false;
				Options.bIgnoreAllPressedKeysUntilRelease = false;
				Subsystem->AddMappingContext(MappingContext, 1, Options);
			}
		}
	}
	
	Cast<APawn>(GetOwner())->Controller = EnteredPawn->Controller;
	EnteredPawn->GetRootComponent()->AttachToComponent(GetOwner()->GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform);
	EnteredPawn->SetActorEnableCollision(false);
	EnteredPawn->SetActorHiddenInGame(true);

	if(CameraComponent)
	{
		CameraComponent->DetermineCameraModeDelegate.BindUObject(this, &ThisClass::DetermineCameraMode);
	}

	bEntered = true;
}

void UVehicleExtensionComponent::OnVehicleExit_Implementation(AActor* CarInstigator, ULyraAbilitySystemComponent* LyraASC)
{
	Execute_OnVehicleExit(GetOwner(), CarInstigator, LyraASC);
	
	if(ULyraInputComponent* LyraIC = Cast<ULyraInputComponent>(CarInstigator->InputComponent))
	{
		LyraIC->RemoveBinds(LoadedBindHandles);
	}

	APlayerController* PawnController = EnteredPawn->GetController<APlayerController>();
	if(PawnController)
	{
		PawnController->SetViewTargetWithBlend(EnteredPawn.Get());
		const ULyraLocalPlayer* LP = Cast<ULyraLocalPlayer>(PawnController->GetLocalPlayer());
		if(LP)
		{
			UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
			if(Subsystem)
			{
				FModifyContextOptions Options;
				Options.bForceImmediately = false;
				Options.bNotifyUserSettings = false;
				Options.bIgnoreAllPressedKeysUntilRelease = false;
				Subsystem->RemoveMappingContext(MappingContext, Options);
			}
		}
	}
	
	LoadedAbilitySetHandles.TakeFromAbilitySystem(LyraASC);
	EnteredPawn->SetActorHiddenInGame(false);
	EnteredPawn->GetRootComponent()->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	EnteredPawn->SetActorEnableCollision(true);
	EnteredPawn->SetActorRotation(FRotator(0.f, 0.f, .0f));
	EnteredPawn = nullptr;

	if(CameraComponent)
	{
		CameraComponent->DetermineCameraModeDelegate.Unbind();
	}
	
	bEntered = false;
}

void UVehicleExtensionComponent::AddToNativeInputHandle(int32 Handle)
{
	LoadedBindHandles.Add(Handle);
}

const ULyraInputConfig* UVehicleExtensionComponent::GetInputConfig() const
{
	return InputConfig;
}

bool UVehicleExtensionComponent::Entered() const
{
	return bEntered;
}

FInteractionOption& UVehicleExtensionComponent::GetInteractionOption()
{
	return Option;
}

void UVehicleExtensionComponent::BeginPlay()
{
	Super::BeginPlay();

	check(AbilitySetToGrant && "AbilitySetToGrant not defined");
	check(InputConfig && "InputConfig not defined");
	check(MappingContext && "MappingContext not defined");
}

void UVehicleExtensionComponent::Input_AbilityInputTagPressed(FGameplayTag InputTag)
{
	if(EnteredPawn.IsExplicitlyNull()) return;
	
	if (const ULyraPawnExtensionComponent* PawnExtComp = ULyraPawnExtensionComponent::FindPawnExtensionComponent(EnteredPawn.Get()))
	{
		if (ULyraAbilitySystemComponent* LyraASC = PawnExtComp->GetLyraAbilitySystemComponent())
		{
			LyraASC->AbilityInputTagPressed(InputTag);
		}
	}	
}

void UVehicleExtensionComponent::Input_AbilityInputTagReleased(FGameplayTag InputTag)
{
	if(EnteredPawn.IsExplicitlyNull()) return;
	if (const ULyraPawnExtensionComponent* PawnExtComp = ULyraPawnExtensionComponent::FindPawnExtensionComponent(EnteredPawn.Get()))
	{
		if (ULyraAbilitySystemComponent* LyraASC = PawnExtComp->GetLyraAbilitySystemComponent())
		{
			LyraASC->AbilityInputTagReleased(InputTag);
		}
	}
}

TSubclassOf<ULyraCameraMode> UVehicleExtensionComponent::DetermineCameraMode() const
{
	return CameraMode ? CameraMode : nullptr;
}
