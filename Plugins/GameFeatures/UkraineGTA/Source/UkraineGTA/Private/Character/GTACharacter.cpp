// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/GTACharacter.h"

#include "BuoyancyComponent.h"
#include "Character/GTAHeroComponent.h"
#include "Character/Components/ArmorHandlerComponent.h"
#include "Character/Components/HungerHandlerComponent.h"
#include "Character/Components/OxygenHandlerComponent.h"
#include "Character/Components/StaminaHandlerComponent.h"
#include "Character/Components/WaterLogicComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Equipment/LyraQuickBarComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Inventory/LyraInventoryManagerComponent.h"
#include "Kismet/KismetSystemLibrary.h"

AGTACharacter::AGTACharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	
	GTAHeroComponent = CreateDefaultSubobject<UGTAHeroComponent>(TEXT("GTAHeroComponent"));
	
	WaterLogicComponent = CreateDefaultSubobject<UWaterLogicComponent>(TEXT("WaterLogicComponent"));
	WaterHeightChecker = CreateDefaultSubobject<USphereComponent>(TEXT("WaterHeightChecker"));
	WaterHeightChecker->OnComponentBeginOverlap.AddUniqueDynamic(this, &AGTACharacter::OnBeginOverlap);
	WaterHeightChecker->OnComponentEndOverlap.AddUniqueDynamic(this, &AGTACharacter::OnEndOverlap);
	WaterHeightChecker->SetupAttachment(RootComponent);

	GetCapsuleComponent()->OnComponentBeginOverlap.AddUniqueDynamic(this, &AGTACharacter::OnBeginOverlap);
	GetCapsuleComponent()->OnComponentEndOverlap.AddUniqueDynamic(this, &AGTACharacter::OnEndOverlap);

	StaminaComponent = CreateDefaultSubobject<UStaminaHandlerComponent>(TEXT("StaminaComponent"));
	HungerComponent  = CreateDefaultSubobject<UHungerHandlerComponent>(TEXT("HungerComponent"));
	ArmorComponent   = CreateDefaultSubobject<UArmorHandlerComponent>(TEXT("ArmorComponent"));
	OxygenComponent  = CreateDefaultSubobject<UOxygenHandlerComponent>(TEXT("OxygenComponent"));

	BuoyancyComponent = CreateDefaultSubobject<UBuoyancyComponent>(TEXT("BuoyancyComponent"));
}

void AGTACharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	const auto& Pontoon = WaterHeightChecker;
	TMap<const UWaterBodyComponent*, float> SplineInputKeys;
	TMap<const UWaterBodyComponent*, float> SplineSegments;
	BuoyancyComponent->GetWaterSplineKey(Pontoon->GetComponentLocation(), SplineInputKeys, SplineSegments);
	
	float WaterHeight = BuoyancyComponent->GetWaterHeight(Pontoon->GetComponentLocation() - GetActorUpVector() * 100.f, SplineInputKeys, -100000.f, true);
	float ActorHeight = WaterHeight - Pontoon->GetComponentLocation().Z;
	
	UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("ActorHeight: %f"), ActorHeight)
		, true, false
		, FLinearColor::Red, 1.f, TEXT("ActorHeight"));
	UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("WaterHeight: %f"), WaterHeight)
	, true, false
	, FLinearColor::Red, 1.f, TEXT("WaterHeight"));

	if(ActorHeight > 0)
	{
		WaterLogicComponent->SetUnderWater(true);
	}
	else if(ActorHeight < 0)
	{
		WaterLogicComponent->SetUnderWater(false);
		if(IsSwimming() && !IsTouchingGroundInWater() && GetCharacterMovement()->Velocity.Z >= 0.f)
		{
			const FVector PontoonLoc = Pontoon->GetComponentLocation();
			const FVector RelPontoonLoc = Pontoon->GetRelativeLocation();
			const FVector ActorLoc = GetActorLocation();
			const float FloatAtHeight = FMath::Lerp(PontoonLoc.Z, WaterHeight + 10.f, 0.5f);
			SetActorLocation(FVector(ActorLoc.X, ActorLoc.Y, FloatAtHeight - RelPontoonLoc.Z));
		}
	}
}

bool AGTACharacter::IsSwimming() const
{
	return WaterLogicComponent->IsSwimming();
}

bool AGTACharacter::AffectedByWater() const
{
	return WaterLogicComponent->AffectedByWater();
}

bool AGTACharacter::IsTouchingGroundInWater() const
{
	return WaterLogicComponent->IsTouchingGroundInWater();
}

bool AGTACharacter::IsUnderWater() const
{
	return WaterLogicComponent->IsUnderWater();
}

void AGTACharacter::AddInitialInventory()
{
	if(!HasAuthority()) return;
	
	auto* Inventory = Controller->GetComponentByClass<ULyraInventoryManagerComponent>();
	auto* QuickBar = Controller->GetComponentByClass<ULyraQuickBarComponent>();
	if(!Inventory || !QuickBar) return;

	Inventory->AddEmptySlots(14);
	for(int slotIndex = 0; slotIndex < InitialInventoryItems.Num(); slotIndex++)
	{
		Inventory->ChangeInventorySlot(InitialInventoryItems[slotIndex], 1);
	}

	QuickBar->SetActiveSlotIndex(0);
	OnInitialUnarmed();
}

void AGTACharacter::OnAbilitySystemInitialized()
{
	Super::OnAbilitySystemInitialized();

	ULyraAbilitySystemComponent* LyraASC = GetLyraAbilitySystemComponent();

	StaminaComponent->InitializeWithAbilitySystem(LyraASC);
	HungerComponent->InitializeWithAbilitySystem(LyraASC);
	ArmorComponent->InitializeWithAbilitySystem(LyraASC);
	OxygenComponent->InitializeWithAbilitySystem(LyraASC);
}

void AGTACharacter::OnAbilitySystemUninitialized()
{
	Super::OnAbilitySystemUninitialized();
	StaminaComponent->UninitializeFromAbilitySystem();
	HungerComponent->UninitializeFromAbilitySystem();
	ArmorComponent->UninitializeFromAbilitySystem();
	OxygenComponent->UninitializeFromAbilitySystem();
}

void AGTACharacter::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent
		, AActor* OtherActor
		, UPrimitiveComponent* OtherComp
		, int32 OtherBodyIndex, bool bFromSweep
		, const FHitResult& SweepResult)
{
	const bool bWaterCollision = OtherComp->GetCollisionProfileName() == FName(TEXT("WaterBodyCollision"));
	
	if(OverlappedComponent == GetCapsuleComponent())
	{
		if(bWaterCollision)
		{
			WaterLogicComponent->SetAffectedByWater(true);
		}
	}
	else if(OverlappedComponent == WaterHeightChecker)
	{
		if(bWaterCollision)
		{
			//WaterLogicComponent->SetSwimming(true);
		}
	}
}

void AGTACharacter::OnEndOverlap(UPrimitiveComponent* OverlappedComponent
		, AActor* OtherActor
		, UPrimitiveComponent* OtherComp
		, int32 OtherBodyIndex)
{
	const bool bWaterCollision = OtherComp->GetCollisionProfileName() == FName(TEXT("WaterBodyCollision"));
	
	if(OverlappedComponent == GetCapsuleComponent())
	{
		if(bWaterCollision)
		{
			WaterLogicComponent->SetAffectedByWater(false);
		}
	}
	else if(OverlappedComponent == WaterHeightChecker)
	{
		if(bWaterCollision)
		{
			//WaterLogicComponent->SetSwimming(false);
		}
	}
}
