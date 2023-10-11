// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Components/WaterLogicComponent.h"

#include "GTAGameplayTags.h"
#include "AbilitySystem/LyraAbilitySystemComponent.h"
#include "Character/GTACharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PhysicsVolume.h"
#include "Kismet/KismetSystemLibrary.h"

UWaterLogicComponent::UWaterLogicComponent(): bCanSwimUp(false), bSwimming(false), bWaterAffection(false),
                                              bTouchingGround(false)
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UWaterLogicComponent::SetAffectedByWater(bool Value)
{
	AGTACharacter* Character = GetOwner<AGTACharacter>();
	if (!Character) return;
	
	ULyraAbilitySystemComponent* LyraASC = Character->GetLyraAbilitySystemComponent();
	if(LyraASC)
	{
		Value
		? LyraASC->AddLooseGameplayTag(GTAGameplayTags::Ability_Behavior_AffectedByWater)
		: LyraASC->RemoveLooseGameplayTag(GTAGameplayTags::Ability_Behavior_AffectedByWater);
	}
	
	bWaterAffection = Value;
	PrimaryComponentTick.SetTickFunctionEnable(Value);
	Character->GetCharacterMovement()->MaxWalkSpeed = Value ? 300.f : 600.f;
}

void UWaterLogicComponent::SetSwimming(bool Value)
{
	AGTACharacter* Character = GetOwner<AGTACharacter>();
	if (!Character) return;

	if(Value)
	{
		bSwimming = Value;
		Character->GetCharacterMovement()->GetPhysicsVolume()->bWaterVolume = Value;
		Character->GetCharacterMovement()->SetMovementMode(Value ? MOVE_Swimming : MOVE_Walking);
		bCanSwimUp = true;
	}
	else
	{
		if(bTouchingGround)
		{
			bSwimming = Value;
			Character->GetCharacterMovement()->GetPhysicsVolume()->bWaterVolume = Value;
			Character->GetCharacterMovement()->SetMovementMode(Value ? MOVE_Swimming : MOVE_Walking);
		}
		else if(bCanSwimUp)
		{
			bCanSwimUp = false;
			Character->GetCharacterMovement()->Velocity.Z = 0.f;
			Character->GetCharacterMovement()->UpdateComponentVelocity();
		}
	}
	
	Character->K2_OnSetSwimming(bSwimming);
}

bool UWaterLogicComponent::IsSwimming() const
{
	return bSwimming;
}

bool UWaterLogicComponent::IsInWater() const
{
	return bWaterAffection;
}

bool UWaterLogicComponent::IsTouchingGroundInWater() const
{
	return bTouchingGround;
}

bool UWaterLogicComponent::CanSwimUp() const
{
	return bCanSwimUp;
}

void UWaterLogicComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	AGTACharacter* Character = GetOwner<AGTACharacter>();
	if (Character)
	{
		FVector StartLocation = Character->GetActorLocation();
		// Bottom of the character's capsule
		FVector EndLocation = Character->GetActorLocation() - FVector(0, 0, Character->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight());

		FHitResult HitResult;
		FCollisionQueryParams CollisionParams;
		CollisionParams.AddIgnoredActor(Character);
		bTouchingGround = UKismetSystemLibrary::SphereTraceSingle(GetWorld()
			, StartLocation
			, EndLocation
			, 8.f
			, UEngineTypes::ConvertToTraceType(ECC_Visibility)
			, false
			, {Character}
			, EDrawDebugTrace::Type::ForOneFrame
			, HitResult
			, false
			, FLinearColor::Green
			, FLinearColor::Red);

		if(!bCanSwimUp && bTouchingGround)
		{
			SetSwimming(false);
		}
	}
}

