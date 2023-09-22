// Fill out your copyright notice in the Description page of Project Settings.


#include "GTAWheeledVehicle.h"

#include "ChaosVehicleMovementComponent.h"
#include "LyraGameplayTags.h"
#include "AbilitySystem/LyraAbilitySystemComponent.h"
#include "Camera/LyraCameraComponent.h"
#include "Camera/LyraCameraMode.h"
#include "Character/LyraPawnExtensionComponent.h"
#include "Input/LyraInputComponent.h"

AGTAWheeledVehicle::AGTAWheeledVehicle(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	CameraComponent = CreateDefaultSubobject<ULyraCameraComponent>(TEXT("CameraCompon"));
	CameraComponent->SetupAttachment(RootComponent);
}

void AGTAWheeledVehicle::GatherInteractionOptions(const FInteractionQuery& InteractQuery,
                                                  FInteractionOptionBuilder& OptionBuilder)
{
	OptionBuilder.AddInteractionOption(Option);
}

void AGTAWheeledVehicle::OnCarEnter(AActor* CarInstigator, ULyraAbilitySystemComponent* LyraASC, ULyraAbilitySet* AbilitySetToGrant, ULyraInputConfig* InputConfig)
{
	if(!AbilitySetToGrant) return;
	AbilitySetToGrant->GiveToAbilitySystem(LyraASC, &LoadedAbilitySetHandles);

	ULyraInputComponent* LyraIC = Cast<ULyraInputComponent>(CarInstigator->InputComponent);
	if(LyraIC)
	{
		EnteredPawn = Cast<APawn>(CarInstigator);
		Controller = EnteredPawn->Controller;
		//EnteredPawn->SetActorHiddenInGame(true);
		EnteredPawn->GetRootComponent()->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);
		EnteredPawn->SetActorEnableCollision(false);
		
		LyraIC->BindAbilityActions(InputConfig, this, &ThisClass::Input_AbilityInputTagPressed, &ThisClass::Input_AbilityInputTagReleased, /*out*/ LoadedBindHandles);
		
		if (const UInputAction* IA = InputConfig->FindNativeInputActionForTag(LyraGameplayTags::InputTag_Native_Move, false))
		{
			LoadedBindHandles.Add(LyraIC->BindAction(IA, ETriggerEvent::Triggered, this, &ThisClass::Input_Move).GetHandle());
		}

		CameraComponent->DetermineCameraModeDelegate.BindUObject(this, &ThisClass::DetermineCameraMode);
	}
}

void AGTAWheeledVehicle::OnCarExit(AActor* CarInstigator, ULyraAbilitySystemComponent* LyraASC)
{
	LoadedAbilitySetHandles.TakeFromAbilitySystem(LyraASC);
	ULyraInputComponent* LyraIC = Cast<ULyraInputComponent>(CarInstigator->InputComponent);
	if(LyraIC)
	{
		LyraIC->RemoveBinds(LoadedBindHandles);
		EnteredPawn->SetActorHiddenInGame(false);
		EnteredPawn->GetRootComponent()->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		EnteredPawn->SetActorEnableCollision(true);
		EnteredPawn->SetActorRotation(FRotator(0.f, 0.f, .0f));
	}
	EnteredPawn = nullptr;
	Controller = nullptr;
	CameraComponent->DetermineCameraModeDelegate.Unbind();
}

TSubclassOf<ULyraCameraMode> AGTAWheeledVehicle::DetermineCameraMode() const
{
	return CarCameraMode ? CarCameraMode : nullptr;
}

void AGTAWheeledVehicle::Input_Move(const FInputActionValue& InputActionValue)
{
	FVector Value = InputActionValue.Get<FVector>();

	GetVehicleMovement()->SetThrottleInput(Value.Y);
	GetVehicleMovement()->SetBrakeInput(Value.Z);
	GetVehicleMovement()->SetSteeringInput(Value.X);
}

void AGTAWheeledVehicle::Input_AbilityInputTagPressed(FGameplayTag InputTag)
{
	if(!EnteredPawn) return;
	
	if (const ULyraPawnExtensionComponent* PawnExtComp = ULyraPawnExtensionComponent::FindPawnExtensionComponent(EnteredPawn))
	{
		if (ULyraAbilitySystemComponent* LyraASC = PawnExtComp->GetLyraAbilitySystemComponent())
		{
			LyraASC->AbilityInputTagPressed(InputTag);
		}
	}	
}

void AGTAWheeledVehicle::Input_AbilityInputTagReleased(FGameplayTag InputTag)
{
	if(!EnteredPawn) return;
	if (const ULyraPawnExtensionComponent* PawnExtComp = ULyraPawnExtensionComponent::FindPawnExtensionComponent(EnteredPawn))
	{
		if (ULyraAbilitySystemComponent* LyraASC = PawnExtComp->GetLyraAbilitySystemComponent())
		{
			LyraASC->AbilityInputTagReleased(InputTag);
		}
	}
}
