// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/GTACharacter.h"

#include "Character/GTAHeroComponent.h"

AGTACharacter::AGTACharacter()
{
	GTAHeroComponent = CreateDefaultSubobject<UGTAHeroComponent>(TEXT("GTAHeroComponent"));
}
