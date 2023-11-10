// Fill out your copyright notice in the Description page of Project Settings.


#include "VehicleExtensionComponent.h"

#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "AbilitySystem/LyraAbilitySystemComponent.h"
#include "Camera/LyraCameraComponent.h"
#include "Camera/LyraCameraMode.h"
#include "Character/LyraPawnExtensionComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Input/LyraInputComponent.h"
#include "Net/UnrealNetwork.h"
#include "Player/LyraLocalPlayer.h"

UE_DEFINE_GAMEPLAY_TAG_STATIC(InputTag_Vehicle_Exit, "InputTag.Vehicle.Exit");
UE_DEFINE_GAMEPLAY_TAG_STATIC(Vehicle_PawnType_Driver, "Vehicle.PawnType.Driver");
UE_DEFINE_GAMEPLAY_TAG_STATIC(Vehicle_PawnType_Passenger, "Vehicle.PawnType.Passenger");

UVehicleExtensionComponent::UVehicleExtensionComponent()
{
	SetIsReplicatedByDefault(true);

	static const auto VehicleExitInputPath = L"/Script/EnhancedInput.InputAction'/GTAVehicles/Input/Actions/IA_VehicleExit.IA_VehicleExit'";
	static const auto PassengerMappingContextPath = L"/Script/EnhancedInput.InputMappingContext'/GTAVehicles/Input/Mappings/IMC_Passenger.IMC_Passenger'";
	static const ConstructorHelpers::FObjectFinder<UInputAction> VehicleExitInputObj(VehicleExitInputPath);
	static const ConstructorHelpers::FObjectFinder<UInputMappingContext> PassengerMappingContextObj(PassengerMappingContextPath);
	VehicleExitInput = VehicleExitInputObj.Object;
	PassengerMappingContext = PassengerMappingContextObj.Object;
}

bool UVehicleExtensionComponent::IsDriver(ULyraAbilitySystemComponent* LyraASC)
{
	return LyraASC->HasMatchingGameplayTag(Vehicle_PawnType_Driver);
}

bool UVehicleExtensionComponent::IsPassenger(ULyraAbilitySystemComponent* LyraASC)
{
	return LyraASC->HasMatchingGameplayTag(Vehicle_PawnType_Passenger);
}

void UVehicleExtensionComponent::SetAbilities(APawn* Pawn, ULyraAbilitySystemComponent* LyraASC, bool Value)
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

void UVehicleExtensionComponent::SetInputs(APawn* Pawn, ULyraAbilitySystemComponent* LyraASC, bool Value)
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

		if(IsPassenger(LyraASC))
		{
			Subsystem->AddMappingContext(PassengerMappingContext, 1, Options);
		}
		if(IsDriver(LyraASC))
		{
			Subsystem->AddMappingContext(DriverMappingContext, 1, Options);
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
	if(Driver && Passenger) return;
	
	auto* VehiclePawn = Cast<APawn>(GetOwner());
	auto* Pawn = Cast<APawn>(PawnInstigator);

	if(!Driver)
	{
		LyraASC->AddLooseGameplayTag(Vehicle_PawnType_Driver);
		Driver = Pawn;
		VehiclePawn->Controller = Pawn->Controller;
		Execute_OnVehicleEnter(VehiclePawn, PawnInstigator, LyraASC);
	}
	else if(!Passenger)
	{
		LyraASC->AddLooseGameplayTag(Vehicle_PawnType_Passenger);
		Passenger = Pawn;
	}

	SetAbilities(Pawn, LyraASC, true);
	SetInputs(Pawn, LyraASC, true);
	SetAttachments(Pawn, LyraASC, true);
}

void UVehicleExtensionComponent::OnVehicleExit_Implementation(AActor* PawnInstigator, ULyraAbilitySystemComponent* LyraASC)
{
	auto* VehiclePawn = Cast<APawn>(GetOwner());
	auto* Pawn = Cast<APawn>(PawnInstigator);

	const bool bDriver = IsDriver(LyraASC);
	const bool bPassenger = IsPassenger(LyraASC);
	
	if(bDriver)
	{
		Execute_OnVehicleExit(VehiclePawn, PawnInstigator, LyraASC);
	}

	SetAbilities(Pawn, LyraASC, false);
	SetInputs(Pawn, LyraASC, false);
	SetAttachments(Pawn, LyraASC, false);
	
	if(bDriver)
	{
		LyraASC->RemoveLooseGameplayTag(Vehicle_PawnType_Driver);
		check(LyraASC->GetGameplayTagCount(Vehicle_PawnType_Driver) == 0);
		Driver = nullptr;
		VehiclePawn->Controller = nullptr;
	}
	if(bPassenger)
	{
		LyraASC->RemoveLooseGameplayTag(Vehicle_PawnType_Passenger);
		check(LyraASC->GetGameplayTagCount(Vehicle_PawnType_Passenger) == 0);
		Passenger = nullptr;
	}
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

bool UVehicleExtensionComponent::DriverEntered() const
{
	return Driver != nullptr;
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
	const auto Controller = Cast<APawn>(GetOwner())->Controller;
	if(!Controller) return;

	const APawn* Pawn = Controller->GetPawn();
	if(!Pawn) return;

	auto* PawnExtComp = ULyraPawnExtensionComponent::FindPawnExtensionComponent(Pawn);
	if (!PawnExtComp) return;

	auto* LyraASC = PawnExtComp->GetLyraAbilitySystemComponent();
	if (!LyraASC) return;
	
	LyraASC->AbilityInputTagPressed(InputTag);
}

void UVehicleExtensionComponent::Input_AbilityInputTagReleased(FGameplayTag InputTag)
{
	const auto Controller = Cast<APawn>(GetOwner())->Controller;
	if(!Controller) return;

	const APawn* Pawn = Controller->GetPawn();
	if(!Pawn) return;

	const auto* PawnExtComp = ULyraPawnExtensionComponent::FindPawnExtensionComponent(Pawn);
	if (!PawnExtComp) return;

	auto* LyraASC = PawnExtComp->GetLyraAbilitySystemComponent();
	if (!LyraASC) return;
	
	LyraASC->AbilityInputTagReleased(InputTag);
}

TSubclassOf<ULyraCameraMode> UVehicleExtensionComponent::DetermineCameraMode() const
{
	return CameraMode ? CameraMode : nullptr;
}
