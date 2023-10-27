// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Components/WaterLogicComponent.h"

#include "BuoyancyComponent.h"
#include "GTAGameplayTags.h"
#include "LyraLogChannels.h"
#include "WaterBodyComponent.h"
#include "AbilitySystem/LyraAbilitySystemComponent.h"
#include "Character/GTACharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"

UWaterLogicComponent::UWaterLogicComponent()
	: bTouchingGround(false)
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UWaterLogicComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UWaterLogicComponent::InitializeWithAbilitySystem(ULyraAbilitySystemComponent* InASC
	, USceneComponent* InPontoon, UBuoyancyComponent* InBuoyancyComponent, UCharacterMovementComponent* InMovement, UCapsuleComponent* InCapsuleComponent)
{
	AActor* Owner = GetOwner();
	check(Owner);
	
	AbilitySystemComponent = InASC;
	if (!AbilitySystemComponent)
	{
		UE_LOG(LogLyra, Error, TEXT("UWaterLogicComponent: Cannot initialize water logic component for owner [%s] with NULL ability system."), *GetNameSafe(Owner));
	}

	Pontoon = InPontoon;
	BuoyancyComponent = InBuoyancyComponent;
	MovementComponent = InMovement;

	CapsuleComponent = InCapsuleComponent;
}

void UWaterLogicComponent::CheckUnderWater()
{
	if(!AbilitySystemComponent) return;
	
	TMap<const UWaterBodyComponent*, float> SplineInputKeys;
	TMap<const UWaterBodyComponent*, float> SplineSegments;
	BuoyancyComponent->GetWaterSplineKey(Pontoon->GetComponentLocation(), SplineInputKeys, SplineSegments);
	
	float WaterHeight = BuoyancyComponent->GetWaterHeight(Pontoon->GetComponentLocation() - GetOwner()->GetActorUpVector() * 100.f
		, SplineInputKeys, -100000.f, true);
	float ActorInWaterHeight = WaterHeight - Pontoon->GetComponentLocation().Z;

	if(DebugWaterInfo)
	{
		UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("ActorInWaterHeight: %f"), ActorInWaterHeight)
	, true, false
	, FLinearColor::Red, 1.f, TEXT("ActorInWaterHeight"));
		UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("WaterHeight: %f"), WaterHeight)
		, true, false
		, FLinearColor::Red, 1.f, TEXT("WaterHeight"));
	}

	if(ActorInWaterHeight > 0)
	{
		SetUnderWater(true);
	}
	else if(ActorInWaterHeight < 0)
	{
		SetUnderWater(false);
		if(IsSwimming() && !bTouchingGround && MovementComponent->Velocity.Z >= 0.f)
		{
			const FVector PontoonLoc = Pontoon->GetComponentLocation();
			const FVector RelPontoonLoc = Pontoon->GetRelativeLocation();
			const FVector ActorLoc = GetOwner()->GetActorLocation();
			const float FloatAtHeight = FMath::Lerp(PontoonLoc.Z, WaterHeight + 10.f, 0.5f);
			GetOwner()->SetActorLocation(FVector(ActorLoc.X, ActorLoc.Y, FloatAtHeight - RelPontoonLoc.Z));
		}
	}
}

void UWaterLogicComponent::SetAffectedByWater(bool Value)
{
	if(Value == AffectedByWater()) return;
	PrimaryComponentTick.SetTickFunctionEnable(Value);
	
	Value
	? AbilitySystemComponent->AddLooseGameplayTag(GTAGameplayTags::Ability_Behavior_Water_AffectedByWater)
	: AbilitySystemComponent->RemoveLooseGameplayTag(GTAGameplayTags::Ability_Behavior_Water_AffectedByWater);
}

void UWaterLogicComponent::SetUnderWater(bool Value)
{
	if(Value == IsUnderWater()) return;
	
	Value
	? AbilitySystemComponent->AddLooseGameplayTag(GTAGameplayTags::Ability_Behavior_Water_UnderWater)
	: AbilitySystemComponent->RemoveLooseGameplayTag(GTAGameplayTags::Ability_Behavior_Water_UnderWater);
}

void UWaterLogicComponent::SetSwimming(bool Value)
{
	if(Value == IsSwimming()) return;
	
	Value
	? AbilitySystemComponent->AddLooseGameplayTag(GTAGameplayTags::Ability_Behavior_Water_Swimming)
	: AbilitySystemComponent->RemoveLooseGameplayTag(GTAGameplayTags::Ability_Behavior_Water_Swimming);
}

bool UWaterLogicComponent::IsSwimming() const
{
	return AbilitySystemComponent->HasMatchingGameplayTag(GTAGameplayTags::Ability_Behavior_Water_Swimming);
}

bool UWaterLogicComponent::AffectedByWater() const
{
	return AbilitySystemComponent->HasMatchingGameplayTag(GTAGameplayTags::Ability_Behavior_Water_AffectedByWater);
}

bool UWaterLogicComponent::IsTouchingGroundInWater() const
{
	return bTouchingGround;
}

bool UWaterLogicComponent::IsUnderWater() const
{
	return AbilitySystemComponent->HasMatchingGameplayTag(GTAGameplayTags::Ability_Behavior_Water_UnderWater);
}

void UWaterLogicComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	const FVector StartLocation = GetOwner()->GetActorLocation();
	const FVector EndLocation = GetOwner()->GetActorLocation() - FVector(0.f, 0.f, CapsuleComponent->GetUnscaledCapsuleHalfHeight());

	FHitResult HitResult;
	bTouchingGround = UKismetSystemLibrary::SphereTraceSingle(GetWorld()
	                                                          , StartLocation
	                                                          , EndLocation
	                                                          , 8.f
	                                                          , UEngineTypes::ConvertToTraceType(ECC_Visibility)
	                                                          , false
	                                                          , {GetOwner()}
	                                                          , EDrawDebugTrace::Type::ForOneFrame
	                                                          , HitResult
	                                                          , false
	                                                          , FLinearColor::Green
	                                                          , FLinearColor::Red);

	CheckUnderWater();

	if(IsSwimming() && bTouchingGround && !IsUnderWater())
	{
		SetSwimming(false);
	}
	else if(!IsSwimming() && IsUnderWater())
	{
		SetSwimming(true);
	}

	if(DebugWaterInfo)
	{
		UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("AffectedByWater: %hhd"), AffectedByWater())
			, true, false
			, FLinearColor::Blue, 1.f, TEXT("AffectedByWater"));

		UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("Swimming: %hhd"), IsSwimming())
			, true, false
			, FLinearColor::Blue, 1.f, TEXT("Swimming"));

		UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("IsTouchingGround: %hhd"), IsTouchingGroundInWater())
			, true, false
			, FLinearColor::Blue, 1.f, TEXT("IsTouchingGround"));

		UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("IsUnderWater: %hhd"), IsUnderWater())
			, true, false
			, FLinearColor::Blue, 1.f, TEXT("IsUnderWater"));
	}
}

