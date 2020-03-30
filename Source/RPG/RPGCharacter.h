// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "RPGCharacter.generated.h"



UCLASS(config=Game)
class ARPGCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
public:
	ARPGCharacter();
	virtual void BeginPlay();

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
		float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
		float BaseLookUpRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Character Movement: Walking")
		float DefaultMaxWalkSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Character Movement: Walking")
		float MaxSprintSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement: Walking")
		bool UnlockedSprint = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
		int Level = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
		int Happiness = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
		int MaxHappiness;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
		int PreviousHappiness = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
		int TempHappiness = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
		float Stamina = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
		float StaminaDecrement = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
		float BreathDrainRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
		float BreathFillRate;
	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
		float MaxStamina = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
		float StaminaDrainRate = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
		float StaminaFillRate = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
		float HappinessMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
		int PerkPoints;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
		int NumberOfLevels = 20;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
		float Breath;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
		bool BreathDraining;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
		bool Alive;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
		TArray<int> HappinessRequirementPerLevel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
		bool HappinessIsChanging = false;

	//FUNCTIONS-------------------------------------------------------

	UFUNCTION(BlueprintPure)
		bool CanAffordStaminaCost(float Value);

	UFUNCTION(BlueprintCallable)
		void SetStamina(float value);

	UFUNCTION(BlueprintCallable)
		void SetHappiness(int value);

	UFUNCTION(BlueprintCallable)
		void AddStamina(float value);

	UFUNCTION(BlueprintCallable)
		void AddHappiness(int Goal, bool CanUseHappinessMultiplier);

	UFUNCTION(BlueprintPure)
		int GetHappinessRequiredForThisLevel();

	//UFUNCTION(BlueprintCallable)
	//	int AddHappinessInc(int Goal, bool CanUseHappinessMultiplier);

	UFUNCTION(BlueprintCallable)
		void AddLevel();

	UFUNCTION(BlueprintCallable)
		void SetMaxSprintSpeed(float Value);

	UFUNCTION(BlueprintCallable)
		void AddToStaminaDrainRate(float Value);
	
	UFUNCTION(BlueprintCallable)
		void ChangeBreath(bool isDraining);


	//EVENTS-------------------------------------------------------------------------

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = StatEvent)
		void StaminaChanged();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = StatEvent)
		void HappinessChanged();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = Movement)
		void CharacterMoved();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = StatEvent)
		void BreathChanged();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = StatEvent)
		void Die();

	//SPRINTING-----------------------------
	void Sprint();

	UFUNCTION(BlueprintCallable)
		void StopSprinting();


	void CheckStamina(float DeltaSeconds);
	void MoveBack(float Value);
	void MyMoveRight(float Value);
	void MyMoveForward(float Value);
	void MyJump();
	void MyStopJumping();
	void AnimateHappinessBar();

	

	int NewGoal = 0;
	bool IsSprinting;


protected:

	/** Resets HMD orientation in VR. */
	void OnResetVR();

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/** 
	 * Called via input to turn at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);


	virtual void Tick(float DeltaSeconds);

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};

