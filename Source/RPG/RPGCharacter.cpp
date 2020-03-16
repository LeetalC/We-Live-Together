// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "RPGCharacter.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Math/Vector.h"
#include "GenericPlatform/GenericPlatformMath.h"
#include "GameFramework/Controller.h"
#include "Engine/World.h"
#include "GameFramework/SpringArmComponent.h"


//////////////////////////////////////////////////////////////////////////
// ARPGCharacter

ARPGCharacter::ARPGCharacter()
{
	// Set size for collision capsule	
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;


	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)
	//AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystem>(TEXT("AbilitySystemComponent"));
	Happiness = 0;
	HappinessMultiplier = 1.0f;
	NumberOfLevels = 20;
	Breath = 100.0f;
	BreathDraining = false;
	Alive = true;


	
}


//////////////////////////////////////////////////////////////////////////
// Input

void ARPGCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ARPGCharacter::MyJump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ARPGCharacter::MyStopJumping);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &ARPGCharacter::Sprint);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &ARPGCharacter::StopSprinting);

	PlayerInputComponent->BindAxis("MoveForward", this, &ARPGCharacter::MyMoveForward);
	PlayerInputComponent->BindAxis("MoveBack", this, &ARPGCharacter::MoveBack);
	PlayerInputComponent->BindAxis("MoveRight", this, &ARPGCharacter::MyMoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ARPGCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ARPGCharacter::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &ARPGCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &ARPGCharacter::TouchStopped);

	// VR headset functionality
	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &ARPGCharacter::OnResetVR);

}

void ARPGCharacter::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}



void ARPGCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
		Jump();
}

void ARPGCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
		StopJumping();
}

void ARPGCharacter::BeginPlay()
{
	Super::BeginPlay();
	DefaultMaxWalkSpeed = 200.0f;
	MaxSprintSpeed = 600.0f;
	PerkPoints = Level;
}
void ARPGCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	CheckStamina(DeltaSeconds);

}



void ARPGCharacter::CheckStamina(float DeltaSeconds) 
{
	if (GetVelocity().Size() >= 400.0f) IsSprinting = true;
	else IsSprinting = false;

	if (Stamina <= 0.0f) 
	{
		StopSprinting();
	}
	if (IsSprinting == true) 
	{
		if (Stamina >= 0.0f)
			Stamina -= StaminaDrainRate * DeltaSeconds;
	}
	else if (Stamina < MaxStamina) Stamina += StaminaFillRate * DeltaSeconds;
}

void ARPGCharacter::ChangeBreath(bool isDraining)
{
	if (isDraining) {
		if (Breath > 0.0f) {
			Breath -= BreathDrainRate;
		}
		else if (Breath <= 0.0f) {
			Alive = false;
			Die();
		}
	}
	else 
	{
		if (Breath <= 100.0f)
			Breath += BreathFillRate;
	}

}


//MOVEMENT INPUT FUNCTIONS-----------------------------------------------------
void ARPGCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ARPGCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ARPGCharacter::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
		CharacterMoved();
	}
}

void ARPGCharacter::MoveRight(float Value)
{
	if ( (Controller != NULL) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
		CharacterMoved();
	}
}

//MY CUSTOM MOVEMENT FUNCTIONS-------

void ARPGCharacter::MoveBack(float Value) {
	if ((Controller != NULL) && (Value != 0.0f))
	{
		GetCharacterMovement()->bOrientRotationToMovement = false;
		// find out which way is forward
		//const FRotator Rotation = Controller->GetControlRotation();
		//const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		//const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector Direction = GetActorForwardVector();
		AddMovementInput(Direction, Value);
		CharacterMoved();
	}
}

void ARPGCharacter::MyMoveForward(float Value) {
	if ((Controller != NULL) && (Value != 0.0f))
	{
		GetCharacterMovement()->bOrientRotationToMovement = true;
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
		CharacterMoved();
	}
}

void ARPGCharacter::MyMoveRight(float Value) {
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is right
		GetCharacterMovement()->bOrientRotationToMovement = true;
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
		CharacterMoved();
	}

}

void ARPGCharacter::MyJump()
{
	if (GetVelocity().Size() > 400.0f)
	{
		bPressedJump = true;
		JumpKeyHoldTime = 0.0f;
		Jump();
	}
}

void ARPGCharacter::MyStopJumping()
{
	bPressedJump = false;
	ResetJumpState();
}

void ARPGCharacter::Sprint()
{

	GetCharacterMovement()->MaxWalkSpeed = MaxSprintSpeed;
}

void ARPGCharacter::StopSprinting()
{
	IsSprinting = false;
	GetCharacterMovement()->MaxWalkSpeed = DefaultMaxWalkSpeed;
}
//END MOVEMENT INPUT FUNCTIONS-----------------------------------------------------

bool ARPGCharacter::CanAffordStaminaCost(float value) {
	if (value < 0.0f) {
		if (Stamina > abs(value)) {
			return true;
		}
	}
	else {
		return true;
	}
	return false;
}


void ARPGCharacter::AddStamina(float value)
{
	Stamina += value;
}

void ARPGCharacter::AddHappiness(int Value, bool CanUseHappinessMultiplier)
{
	if (CanUseHappinessMultiplier)
	{
		Happiness += (Value * HappinessMultiplier);
	}
	else {
		Happiness += Value;
	}
	TempHappiness += Happiness;
	

	HappinessChanged();
	
}

void ARPGCharacter::SetStamina(float value)
{
	Stamina = value;

}
void ARPGCharacter::SetHappiness(int value)
{
	Happiness = value;
}

void ARPGCharacter::AddLevel()
{
	Level++;
	PerkPoints++;
}


void ARPGCharacter::SetMaxSprintSpeed(float Value)
{
	MaxSprintSpeed = Value;
}

void ARPGCharacter::AddToStaminaDrainRate(float Value)
{
	StaminaDrainRate += Value;
}
