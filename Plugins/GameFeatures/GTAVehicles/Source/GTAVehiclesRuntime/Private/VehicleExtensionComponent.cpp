// Fill out your copyright notice in the Description page of Project Settings.


#include "VehicleExtensionComponent.h"

#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "AbilitySystem/LyraAbilitySystemComponent.h"
#include "Camera/LyraCameraComponent.h"
#include "Camera/LyraCameraMode.h"
#include "Input/LyraInputComponent.h"
#include "Net/UnrealNetwork.h"
#include "Player/LyraLocalPlayer.h"

UE_DEFINE_GAMEPLAY_TAG_STATIC(InputTag_Vehicle_Exit, "InputTag.Vehicle.Exit");

UVehicleExtensionComponent::UVehicleExtensionComponent() : PawnType(Vehicle::None)
{
	SetIsReplicatedByDefault(true);

	static const auto VehicleExitInputPath = L"/Script/EnhancedInput.InputAction'/GTAVehicles/Input/Actions/IA_VehicleExit.IA_VehicleExit'";
	static const auto PassengerMappingContextPath = L"/Script/EnhancedInput.InputMappingContext'/GTAVehicles/Input/Mappings/IMC_Passenger.IMC_Passenger'";
	static const ConstructorHelpers::FObjectFinder<UInputAction> VehicleExitInputObj(VehicleExitInputPath);
	static const ConstructorHelpers::FObjectFinder<UInputMappingContext> PassengerMappingContextObj(PassengerMappingContextPath);
	VehicleExitInput = VehicleExitInputObj.Object;
	PassengerMappingContext = PassengerMappingContextObj.Object;
}

void UVehicleExtensionComponent::SetAbilities(APawn* Pawn, bool Value)
{
	ULyraInputComponent* LyraIC = Cast<ULyraInputComponent>(Pawn->InputComponent);
	if(!LyraIC) return;
	
	if(Value)
	{
		if(AbilitySetToGrant)
		{
			AbilitySetToGrant->GiveToAbilitySystem(LyraASC, &LoadedAbilitySetHandles);
		}

		LyraIC->BindAbilityActions(InputConfig
			, this, &ThisClass::Input_AbilityInputTagPressed, &ThisClass::Input_AbilityInputTagReleased, /*out*/ LoadedBindHandles);
		
		LoadedBindHandles.Add(LyraIC->BindAction(VehicleExitInput
			, ETriggerEvent::Triggered, this, &ThisClass::Input_AbilityInputTagPressed, InputTag_Vehicle_Exit.GetTag()).GetHandle());
		LoadedBindHandles.Add(LyraIC->BindAction(VehicleExitInput
			, ETriggerEvent::Triggered, this, &ThisClass::Input_AbilityInputTagReleased, InputTag_Vehicle_Exit.GetTag()).GetHandle());
	}
	else
	{
		LyraIC->RemoveBinds(LoadedBindHandles);
		LoadedAbilitySetHandles.TakeFromAbilitySystem(LyraASC);
	}
}

void UVehicleExtensionComponent::SetInputs(APawn* Pawn, bool Value)
{
	APlayerController* PawnController = Pawn->GetController<APlayerController>();
	if(!PawnController) return;
	
	const ULyraLocalPlayer* LP = Cast<ULyraLocalPlayer>(PawnController->GetLocalPlayer());
	if(!LP) return;
	
	UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check(Subsystem);

	FModifyContextOptions Options;
	Options.bForceImmediately = false;
	Options.bNotifyUserSettings = false;
	Options.bIgnoreAllPressedKeysUntilRelease = false;

	
	PawnController->SetViewTargetWithBlend(Value ? GetOwner() : Pawn);
	if(Value)
	{
		if(CameraComponent)
		{
			CameraComponent->DetermineCameraModeDelegate.BindUObject(this, &ThisClass::DetermineCameraMode);
		}

		switch (PawnType)
		{
		case Vehicle::None:
			break;
		case Vehicle::Driver:
			Subsystem->AddMappingContext(DriverMappingContext, 1, Options);
			break;
		case Vehicle::Passenger:
			Subsystem->AddMappingContext(PassengerMappingContext, 1, Options);
			break;
		default: ;
		}
	}
	else
	{
		if(CameraComponent)
		{
			CameraComponent->DetermineCameraModeDelegate.Unbind();
		}
		
		Subsystem->RemoveMappingContext(DriverMappingContext, Options);
		Subsystem->RemoveMappingContext(PassengerMappingContext, Options);
	}
}

void UVehicleExtensionComponent::SetAttachments(APawn* Pawn, bool Value)
{
	//SetPawnHidden(Pawn, Value);

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

void UVehicleExtensionComponent::OnVehicleEnter_Implementation(AActor* InInstigator, ULyraAbilitySystemComponent* InLyraASC)
{
	if(Driver && Passenger) return;
	
	LyraASC = InLyraASC;
	auto* ComponentOwner = Cast<APawn>(GetOwner());
	auto* Pawn = Cast<APawn>(InInstigator);

	if(!Driver)
	{
		PawnType = Vehicle::Driver;
		Driver = Pawn;
		ComponentOwner->Controller = Pawn->Controller;
		Execute_OnVehicleEnter(GetOwner(), InInstigator, InLyraASC);
	}
	else
	{
		PawnType = Vehicle::Passenger;
		Passenger = Pawn;
		if(!InInstigator->HasAuthority() || InInstigator->GetNetMode() == NM_Standalone)
		{
			ComponentOwner->Controller = Pawn->Controller;
		}
	}

	SetAbilities(Pawn, true);
	SetInputs(Pawn, true);
	SetAttachments(Pawn, true);
}

void UVehicleExtensionComponent::OnVehicleExit_Implementation(AActor* InInstigator, ULyraAbilitySystemComponent* InLyraASC)
{
	auto* ComponentOwner = Cast<APawn>(GetOwner());
	auto* Pawn = Cast<APawn>(InInstigator);
	
	if(PawnType == Vehicle::Driver)
	{
		Execute_OnVehicleExit(GetOwner(), InInstigator, InLyraASC);
	}

	SetAbilities(Pawn, false);
	SetInputs(Pawn, false);
	SetAttachments(Pawn, false);
	
	if(PawnType == Vehicle::Driver)
	{
		Driver = nullptr;
		ComponentOwner->Controller = nullptr;
	}
	if(PawnType == Vehicle::Passenger)
	{
		Passenger = nullptr;
		if(!InInstigator->HasAuthority() || InInstigator->GetNetMode() == NM_Standalone)
		{
			ComponentOwner->Controller = nullptr;
		}
	}
	
	PawnType = Vehicle::None;
	LyraASC = nullptr;
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

void UVehicleExtensionComponent::BeginPlay()
{
	Super::BeginPlay();

	check(AbilitySetToGrant && "AbilitySetToGrant not defined");
	check(InputConfig && "InputConfig not defined");
	check(DriverMappingContext && "MappingContext not defined");
}


void UVehicleExtensionComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, Driver);
	DOREPLIFETIME(ThisClass, Passenger);
}

void UVehicleExtensionComponent::Input_AbilityInputTagPressed(FGameplayTag InputTag)
{
	LyraASC->AbilityInputTagPressed(InputTag);
}

void UVehicleExtensionComponent::Input_AbilityInputTagReleased(FGameplayTag InputTag)
{
	LyraASC->AbilityInputTagReleased(InputTag);
}

TSubclassOf<ULyraCameraMode> UVehicleExtensionComponent::DetermineCameraMode() const
{
	return CameraMode ? CameraMode : nullptr;
}
