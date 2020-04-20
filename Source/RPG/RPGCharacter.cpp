// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "RPGCharacter.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Engine/Engine.h"
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

}


//////////////////////////////////////////////////////////////////////////
// Input

void ARPGCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ARPGCharacter::MyJump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &ARPGCharacter::Sprint);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &ARPGCharacter::StopSprinting);


	PlayerInputComponent->BindAxis("MoveForward", this, &ARPGCharacter::MyMoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ARPGCharacter::MyMoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ARPGCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ARPGCharacter::LookUpAtRate);

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
	CheckStamina();
	AnimateHappinessBar();
}

//--------------------Stamina Functions--------------------------
void ARPGCharacter::RestoreStamina()
{
	StaminaFillRate = StaminaFillRateDefault * StaminaFillMultiplier;
}

bool ARPGCharacter::CanAffordStaminaCost(float Value) {
	if (Value < 0.0f) {
		if (Stamina > abs(Value)) {
			return true;
		}
	}
	else {
		return true;
	}
	return false;
}

void ARPGCharacter::AddStamina(float Value)
{
	Stamina += Value;
}

void ARPGCharacter::SetStamina(float Value)
{
	Stamina = Value;
}

void ARPGCharacter::AddToStaminaDrainRate(float Value)
{
	StaminaDrainRate += Value;
}
void ARPGCharacter::CheckForLevelUp() {
	if (Happiness >= GetHappinessRequiredForThisLevel()) 
	{
		Happiness -= GetHappinessRequiredForThisLevel();
		AddLevelAndPerkPoint();
	}
}

void ARPGCharacter::CheckStamina()
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
			Stamina -= StaminaDrainRate * GetWorld()->GetDeltaSeconds();
	}
	else if (Stamina < MaxStamina) Stamina += StaminaFillRate * GetWorld()->GetDeltaSeconds();
}

//---------------Happiness Functions-------------------------------
void ARPGCharacter::AddHappiness(int Goal, bool CanUseHappinessMultiplier)
{
	if (CanUseHappinessMultiplier)
	{
		NewGoal = Goal * HappinessMultiplier;
	}
	else
	{
		NewGoal = Goal;
	}

	HappinessIsChanging = true;
}

int ARPGCharacter::GetHappinessRequiredForThisLevel()
{
	return HappinessRequirementPerLevel[Level];
}

void ARPGCharacter::SetHappiness(int Value)
{
	Happiness = Value;
}

void ARPGCharacter::AddLevelAndPerkPoint()
{
	Level++;
	PerkPoints++;
	LeveledUp();
}

void ARPGCharacter::AnimateHappinessBar() {
	//animating the fill of the happiness bar
	if (HappinessIsChanging) {
		if (NewGoal > 0) {
			NewGoal -= 5;
			Happiness += 5;
			HappinessChanged();
			CheckForLevelUp();
		}
		else {
			NewGoal = 0;
			HappinessIsChanging = false;
		}
	}
	
}
//-------------------Breath-----------------------------------
void ARPGCharacter::ChangeBreath(bool IsDraining)
{
	if (IsDraining) {
		if (Breath > 0.0f) {
			Breath -= BreathDrainRate;
		}
		else if (Breath <= 0.0f) {
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

void ARPGCharacter::MyMoveForward(float Value) {
	if ((Controller != NULL) && (Value != 0.0f) && !InputBlocked)
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
	if ((Controller != NULL) && (Value != 0.0f) && !InputBlocked)
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

//MY CUSTOM MOVEMENT FUNCTIONS-------
//Moving backwards is disabled, need to rework it
void ARPGCharacter::MoveBack() {
	if ((Controller != NULL))
	{
		InputBlocked = true;
		GetCharacterMovement()->bOrientRotationToMovement = false;
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		//const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector Direction = GetActorForwardVector();
		AddMovementInput(Direction, 1.0f);
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
	if(UnlockedSprint) GetCharacterMovement()->MaxWalkSpeed = MaxSprintSpeed;
}

void ARPGCharacter::StopSprinting()
{
	if (UnlockedSprint) {
		IsSprinting = false;
		GetCharacterMovement()->MaxWalkSpeed = DefaultMaxWalkSpeed;
	}
}

void ARPGCharacter::SetMaxSprintSpeed(float Value)
{
	MaxSprintSpeed = Value;
}
//END MOVEMENT INPUT FUNCTIONS-----------------------------------------------------
