// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Components/WaterLogicComponent.h"

#include "GTAGameplayTags.h"
#include "AbilitySystem/LyraAbilitySystemComponent.h"
#include "Character/GTACharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PhysicsVolume.h"
#include "Kismet/KismetSystemLibrary.h"

UWaterLogicComponent::UWaterLogicComponent()
	: bSwimming(false), bWaterAffection(false), bTouchingGround(false), bUnderWater(false)
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UWaterLogicComponent::SetAffectedByWater(bool Value)
{
	if(Value == bWaterAffection) return;
	bWaterAffection = Value;
	PrimaryComponentTick.SetTickFunctionEnable(Value);
	
	AGTACharacter* Character = GetOwner<AGTACharacter>();
	if (!Character) return;

	if(ULyraAbilitySystemComponent* LyraASC = Character->GetLyraAbilitySystemComponent())
	{
		Value
		? LyraASC->AddLooseGameplayTag(GTAGameplayTags::Ability_Behavior_AffectedByWater)
		: LyraASC->RemoveLooseGameplayTag(GTAGameplayTags::Ability_Behavior_AffectedByWater);
	}
	
	Character->GetCharacterMovement()->MaxWalkSpeed = Value ? 300.f : 600.f;
}

void UWaterLogicComponent::SetUnderWater(bool Value)
{
	if(Value == bUnderWater) return;
	bUnderWater = Value;
	
	const AGTACharacter* Character = GetOwner<AGTACharacter>();
	if (!Character) return;

	if(ULyraAbilitySystemComponent* LyraASC = Character->GetLyraAbilitySystemComponent())
	{
		Value
		? LyraASC->AddLooseGameplayTag(GTAGameplayTags::Ability_Behavior_UnderWater)
		: LyraASC->RemoveLooseGameplayTag(GTAGameplayTags::Ability_Behavior_UnderWater, LyraASC->GetTagCount(GTAGameplayTags::Ability_Behavior_UnderWater));
	}
	
	if(!bUnderWater && bSwimming)
	{
		Character->GetCharacterMovement()->Velocity.Z = 0.f;
		Character->GetCharacterMovement()->UpdateComponentVelocity();
	}
}

void UWaterLogicComponent::SetSwimming(bool Value)
{
	if(Value == bSwimming) return;
	bSwimming = Value;
	
	AGTACharacter* Character = GetOwner<AGTACharacter>();
	if (!Character) return;
	
	Character->GetCharacterMovement()->GetPhysicsVolume()->bWaterVolume = Value;
	Character->GetCharacterMovement()->SetMovementMode(Value ? MOVE_Swimming : MOVE_Walking);

	Character->K2_OnSetSwimming(Value);
}

bool UWaterLogicComponent::IsSwimming() const
{
	return bSwimming;
}

bool UWaterLogicComponent::AffectedByWater() const
{
	return bWaterAffection;
}

bool UWaterLogicComponent::IsTouchingGroundInWater() const
{
	return bTouchingGround;
}

bool UWaterLogicComponent::IsUnderWater() const
{
	return bUnderWater;
}

void UWaterLogicComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	AGTACharacter* Character = GetOwner<AGTACharacter>();
	if (!Character) return;
	
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

	if(bSwimming && bTouchingGround && !bUnderWater)
	{
		SetSwimming(false);
	}
	else if(!bSwimming && bUnderWater)
	{
		SetSwimming(true);
	}
}

