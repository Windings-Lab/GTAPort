// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/GTACharacter.h"

#include "Character/GTAHeroComponent.h"

AGTACharacter::AGTACharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	GTAHeroComponent = CreateDefaultSubobject<UGTAHeroComponent>(TEXT("GTAHeroComponent"));
}
