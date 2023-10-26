// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Components/ArmorHandlerComponent.h"

#include "LyraLogChannels.h"
#include "AbilitySystem/LyraAbilitySystemComponent.h"
#include "AbilitySystem/Attributes/ArmorAttributeSet.h"


UArmorHandlerComponent::UArmorHandlerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UArmorHandlerComponent::AfterASCInit()
{
	AActor* Owner = GetOwner();

	AttributeSet = AbilitySystemComponent->GetSet<UArmorAttributeSet>();
	if (!AttributeSet)
	{
		UE_LOG(LogLyra, Error, TEXT("ArmorHandlerComponent: Cannot initialize armor handler component for owner [%s] with NULL GTACombatSet on the ability system."), *GetNameSafe(Owner));
	}
}
