// Fill out your copyright notice in the Description page of Project Settings.


#include "Helicopters/HelicopterPawn.h"

#include "GTAVehicleGameplayTags.h"
#include "VehicleExtensionComponent.h"
#include "Camera/LyraCameraComponent.h"
#include "Camera/LyraCameraMode.h"
#include "Input/LyraInputComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

AHelicopterPawn::AHelicopterPawn()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	HelicopterMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Body"));
	RootComponent = HelicopterMesh;

	VehicleExtensionComponent = CreateDefaultSubobject<UVehicleExtensionComponent>(TEXT("VehicleExtensionComponent"));
	VehicleExtensionComponent->CameraComponent = CreateDefaultSubobject<ULyraCameraComponent>(TEXT("CameraComponent"));
	VehicleExtensionComponent->CameraComponent->SetupAttachment(RootComponent);

	MaxBladeRotationSpeed = 1500.f;
	ThrottleUpSpeed = 200.f;
	TurnSpeed = 0.1;
	BlurBladeSpeed = 900.f;
}

void AHelicopterPawn::GatherInteractionOptions(const FInteractionQuery& InteractQuery,
	FInteractionOptionBuilder& OptionBuilder)
{
	OptionBuilder.AddInteractionOption(VehicleExtensionComponent->GetInteractionOption());
}

void AHelicopterPawn::CustomizeInteractionEventData(const FGameplayTag& InteractionEventTag,
	FGameplayEventData& InOutEventData)
{
	InOutEventData.OptionalObject = VehicleExtensionComponent;
}

void AHelicopterPawn::OnVehicleEnter_Implementation(AActor* CarInstigator, ULyraAbilitySystemComponent* LyraASC)
{
	if(ULyraInputComponent* LyraIC = Cast<ULyraInputComponent>(CarInstigator->InputComponent))
	{
		const ULyraInputConfig* InputConfig = VehicleExtensionComponent->GetInputConfig();
		if (const UInputAction* IA = InputConfig->FindNativeInputActionForTag(GTAVehicleGameplayTags::InputTag_Native_Aircraft_Thrust, false))
		{
			VehicleExtensionComponent->AddToNativeInputHandle(
				LyraIC->BindAction(IA, ETriggerEvent::Triggered, this, &ThisClass::Input_Throttle).GetHandle());
		}

		if (const UInputAction* IA = InputConfig->FindNativeInputActionForTag(GTAVehicleGameplayTags::InputTag_Native_Aircraft_Yaw, false))
		{
			VehicleExtensionComponent->AddToNativeInputHandle(
				LyraIC->BindAction(IA, ETriggerEvent::Triggered, this, &ThisClass::Input_Yaw).GetHandle());
		}

		if (const UInputAction* IA = InputConfig->FindNativeInputActionForTag(GTAVehicleGameplayTags::InputTag_Native_Aircraft_Pitch, false))
		{
			VehicleExtensionComponent->AddToNativeInputHandle(
				LyraIC->BindAction(IA, ETriggerEvent::Triggered, this, &ThisClass::Input_Pitch).GetHandle());
		}

		if (const UInputAction* IA = InputConfig->FindNativeInputActionForTag(GTAVehicleGameplayTags::InputTag_Native_Aircraft_Roll, false))
		{
			VehicleExtensionComponent->AddToNativeInputHandle(
				LyraIC->BindAction(IA, ETriggerEvent::Triggered, this, &ThisClass::Input_Roll).GetHandle());
		}
	}
}

void AHelicopterPawn::OnVehicleExit_Implementation(AActor* CarInstigator, ULyraAbilitySystemComponent* LyraASC)
{
	Controller = nullptr;
}

void AHelicopterPawn::BeginPlay()
{
	Super::BeginPlay();
	UpdatePreviousValues();
}

void AHelicopterPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CurrentBladeRotationSpeed = FMath::FInterpTo(CurrentBladeRotationSpeed, TargetBladeRotationSpeed, DeltaTime, 1.f);
	RotateBlades(DeltaTime);
		
	AddActorWorldOffset(V + (A * DeltaTime), true);

	AddActorWorldRotation(FRotator(0.f, CurrentTurn, 0.f));
	FRotator NewRotation = GetActorRotation();
	NewRotation = FRotator(NewRotation.Pitch, NewRotation.Yaw, CurrentTurn * 40.f);
	SetActorRotation(NewRotation);

	if(UKismetMathLibrary::InRange_FloatFloat(GetActorRotation().Pitch * CurrentPitch, -89.f, 89.f, true, true))
	{
		AddActorLocalRotation(FRotator(CurrentPitch, 0.f, 0.f));
	}
	
	PrintVariables();
}

void AHelicopterPawn::RotateBlades(float DeltaTime)
{
	K2_OnRotateBlades(DeltaTime);
}

float AHelicopterPawn::GetTurnSpeed()
{
	float Clamped = FMath::GetMappedRangeValueClamped(FVector2D(0.f, 400.f)
		, FVector2D(0.f, 1.f), GetCurrentLift());

	return TurnSpeed * Clamped;
}

float AHelicopterPawn::GetCurrentLift()
{
	return LiftCurve->GetFloatValue(CurrentBladeRotationSpeed);
}

void AHelicopterPawn::UpdatePreviousValues()
{
	PreviousPosition = GetActorLocation();
	Vp = V;
	Ap = A;
}

void AHelicopterPawn::PrintVariables()
{
	if(!VehicleExtensionComponent->Entered()) return;

	UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("CurrentBladeRotationSpeed: %f"), CurrentBladeRotationSpeed)
	, true, false, FLinearColor::Green, 1.f, TEXT("CurrentBladeRotationSpeed"));
}

void AHelicopterPawn::Input_Throttle(const FInputActionValue& InputActionValue)
{
	float InputValue = InputActionValue.Get<float>();
	float DT = GetWorld()->GetDeltaSeconds();

	float NewTargetBladeRotationSpeed = InputValue * DT * ThrottleUpSpeed;
	NewTargetBladeRotationSpeed += TargetBladeRotationSpeed;
	TargetBladeRotationSpeed = FMath::Clamp(NewTargetBladeRotationSpeed, 0.f, MaxBladeRotationSpeed);

	V = (GetActorLocation() - PreviousPosition) * DT;
	A = (V - Vp) * DT;

	UpdatePreviousValues();

	// Apply Gravity
	A += FVector(0.f, 0.f, -981.f);

	// Apply Lift
	A += (GetActorUpVector() * GetCurrentLift());

	// Simulate forward acceleration
	FVector CustomUpVector = GetActorUpVector();
	CustomUpVector.Z = 0.f;
	A += GetCurrentLift() * CustomUpVector;
}

void AHelicopterPawn::Input_Yaw(const FInputActionValue& InputActionValue)
{
	float InputValue = InputActionValue.Get<float>();
	float DT = GetWorld()->GetDeltaSeconds();

	CurrentTurn = FMath::FInterpTo(CurrentTurn, GetTurnSpeed() * InputValue, DT, 2.f);
}

void AHelicopterPawn::Input_Pitch(const FInputActionValue& InputActionValue)
{
	float InputValue = InputActionValue.Get<float>();
	float DT = GetWorld()->GetDeltaSeconds();

	CurrentPitch = FMath::FInterpTo(CurrentPitch, GetTurnSpeed() * InputValue, DT, 2.f);
}

void AHelicopterPawn::Input_Roll(const FInputActionValue& InputActionValue)
{
	float Value = InputActionValue.Get<float>();
}

