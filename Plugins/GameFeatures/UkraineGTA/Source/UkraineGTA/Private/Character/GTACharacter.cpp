// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/GTACharacter.h"

#include "GTAGameplayTags.h"
#include "AbilitySystem/LyraAbilitySystemComponent.h"
#include "AbilitySystem/Attributes/GTACombatSet.h"
#include "Character/GTAHeroComponent.h"
#include "Character/Components/WaterLogicComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"

AGTACharacter::AGTACharacter()
{
	GTAHeroComponent = CreateDefaultSubobject<UGTAHeroComponent>(TEXT("GTAHeroComponent"));
	WaterLogicComponent = CreateDefaultSubobject<UWaterLogicComponent>(TEXT("WaterLogicComponent"));

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("WaterHeightChecker"));
	SphereComponent->SetupAttachment(RootComponent);

	SphereComponent->OnComponentBeginOverlap.AddUniqueDynamic(this, &AGTACharacter::OnBeginOverlap);
	SphereComponent->OnComponentEndOverlap.AddUniqueDynamic(this, &AGTACharacter::OnEndOverlap);

	GetCapsuleComponent()->OnComponentBeginOverlap.AddUniqueDynamic(this, &AGTACharacter::OnBeginOverlap);
	GetCapsuleComponent()->OnComponentEndOverlap.AddUniqueDynamic(this, &AGTACharacter::OnEndOverlap);
}

bool AGTACharacter::IsSwimming() const
{
	return WaterLogicComponent->IsSwimming();
}

bool AGTACharacter::IsInWater() const
{
	return WaterLogicComponent->IsInWater();
}

bool AGTACharacter::IsTouchingGroundInWater() const
{
	return WaterLogicComponent->IsTouchingGroundInWater();
}

bool AGTACharacter::CanSwimUp() const
{
	return WaterLogicComponent->bCanSwimUp;
}

void AGTACharacter::OnAbilitySystemInitialized()
{
	Super::OnAbilitySystemInitialized();

	ULyraAbilitySystemComponent* LyraASC = GetLyraAbilitySystemComponent();

	GTACombatSet = LyraASC->GetSet<UGTACombatSet>();
	check(GTACombatSet)
	
	if (!GTACombatSet)
	{
		return;
	}

	GTACombatSet->OnLowStamina.AddUObject(this, &ThisClass::HandleLowStamina);
	GTACombatSet->OnLowHunger.AddUObject(this, &AGTACharacter::HandleLowHunger);
}

void AGTACharacter::OnAbilitySystemUninitialized()
{
	Super::OnAbilitySystemUninitialized();
}

void AGTACharacter::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent
		, AActor* OtherActor
		, UPrimitiveComponent* OtherComp
		, int32 OtherBodyIndex, bool bFromSweep
		, const FHitResult& SweepResult)
{
	if(OverlappedComponent == GetCapsuleComponent())
	{
		if(OtherComp->GetCollisionProfileName() == FName(TEXT("WaterBodyCollision")))
		{
			if(!IsInWater())
				WaterLogicComponent->SetAffectedByWater(true);
		}
	}
	else if(OverlappedComponent == SphereComponent)
	{
		if(OtherComp->GetCollisionProfileName() == FName(TEXT("WaterBodyCollision")))
		{
			if(!IsSwimming() || !CanSwimUp())
				WaterLogicComponent->SetSwimming(true);
		}
	}
}

void AGTACharacter::OnEndOverlap(UPrimitiveComponent* OverlappedComponent
		, AActor* OtherActor
		, UPrimitiveComponent* OtherComp
		, int32 OtherBodyIndex)
{
	if(OverlappedComponent == GetCapsuleComponent())
	{
		if(OtherComp->GetCollisionProfileName() == FName(TEXT("WaterBodyCollision")))
		{
			if(IsInWater())
				WaterLogicComponent->SetAffectedByWater(false);
		}
	}
	else if(OverlappedComponent == SphereComponent)
	{
		if(OtherComp->GetCollisionProfileName() == FName(TEXT("WaterBodyCollision")))
		{
			if(IsSwimming())
				WaterLogicComponent->SetSwimming(false);
		}
	}
}

void AGTACharacter::HandleLowStamina(AActor* StaminaInstigator, AActor* StaminaCauser, const FGameplayEffectSpec* Spec,
                                     float Magnitude, float OldValue, float NewValue)
{
	ULyraAbilitySystemComponent* LyraASC = GetLyraAbilitySystemComponent();
	{
		FGameplayEventData Payload;

		Payload.EventTag = GTAGameplayTags::Ability_Debuff_LowStamina;
		Payload.Instigator = StaminaInstigator;
		Payload.Target = StaminaCauser;
		Payload.OptionalObject = Spec->Def;
		Payload.ContextHandle = Spec->GetEffectContext();
		Payload.InstigatorTags = *Spec->CapturedSourceTags.GetAggregatedTags();
		Payload.TargetTags = *Spec->CapturedTargetTags.GetAggregatedTags();
		Payload.EventMagnitude = Magnitude;

		FScopedPredictionWindow NewScopedWindow(LyraASC, true);
		LyraASC->HandleGameplayEvent(Payload.EventTag, &Payload);
	}
}

void AGTACharacter::HandleLowHunger(AActor* HungerInstigator, AActor* HungerCauser, const FGameplayEffectSpec* Spec,
	float Magnitude, float OldValue, float NewValue)
{
 	ULyraAbilitySystemComponent* LyraASC = GetLyraAbilitySystemComponent();
	{
		FGameplayEventData Payload;
		Payload.EventTag = GTAGameplayTags::Ability_Debuff_LowHunger;
		Payload.Instigator = HungerInstigator;
		Payload.Target = HungerCauser;
		Payload.OptionalObject = Spec->Def;
		Payload.ContextHandle = Spec->GetEffectContext();
		Payload.InstigatorTags = *Spec->CapturedSourceTags.GetAggregatedTags();
		Payload.TargetTags = *Spec->CapturedTargetTags.GetAggregatedTags();
		Payload.EventMagnitude = Magnitude;

		FScopedPredictionWindow NewScopedWindow(LyraASC, true);
		LyraASC->HandleGameplayEvent(Payload.EventTag, &Payload);
	}
}
