// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/GTAPlayerState.h"

AGTAPlayerState::AGTAPlayerState(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void AGTAPlayerState::BeginPlay()
{
	Super::BeginPlay();
}

void AGTAPlayerState::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}
