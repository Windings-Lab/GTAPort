// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/DebugInfoWidget.h"

#include "CommonRichTextBlock.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

void UDebugInfoWidget::TickCPP(float DeltaSeconds)
{
	ACharacter* Character = Cast<ACharacter>(GetOwningPlayerPawn());
	check(Character)
	
	FVector Location = Character->GetActorLocation();
	float Speed = Character->GetCharacterMovement()->Velocity.Length();
	
	Coordinates->SetText(FText::FromString(FString::Printf(TEXT("XYZ: %.5f / %.5f / %.2f"), Location.X, Location.Y, Location.Z)));
	CharacterSpeed->SetText(FText::FromString(FString::Printf(TEXT("Speed: %.0f"), Speed)));
}
