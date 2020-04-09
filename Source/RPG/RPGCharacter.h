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
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseLookUpRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Character Movement: Walking")
		float DefaultMaxWalkSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Character Movement: Walking")
		float MaxSprintSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement: Walking")
		bool UnlockedSprint = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement: Walking")
		bool InputBlocked = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
		int Level = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
		int PerkPoints;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
		TArray<int> HappinessRequirementPerLevel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
		bool HappinessIsChanging = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
		int Happiness = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
		float HappinessMultiplier = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
		float Stamina = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
		float StaminaDecrement = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
		float BreathDrainRate = .6f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
		float BreathFillRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
		float Breath = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
		bool BreathDraining = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
		float BreathFillRateDefault = 1.1f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
		float MaxBreath = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
		float MaxStamina = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
		float StaminaDrainRate = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
		float StaminaFillRate = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
		float StaminaFillRateDefault = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
		float StaminaFillMultiplier = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
		bool JumpAllowed = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
		bool DashAllowed = true;




	//FUNCTIONS-------------------------------------------------------

	UFUNCTION(BlueprintPure)
		bool CanAffordStaminaCost(float Value);

	UFUNCTION(BlueprintCallable)
		void RestoreStamina();

	UFUNCTION(BlueprintCallable)
		void SetStamina(float Value);

	UFUNCTION(BlueprintCallable)
		void SetHappiness(int Value);

	UFUNCTION(BlueprintCallable)
		void AddStamina(float Value);

	UFUNCTION(BlueprintCallable)
		void AddHappiness(int Goal, bool CanUseHappinessMultiplier);

	UFUNCTION(BlueprintPure)
		int GetHappinessRequiredForThisLevel();

	UFUNCTION(BlueprintCallable)
		void AddLevel();

	UFUNCTION(BlueprintCallable)
		void SetMaxSprintSpeed(float Value);

	UFUNCTION(BlueprintCallable)
		void AddToStaminaDrainRate(float Value);
	
	UFUNCTION(BlueprintCallable)
		void ChangeBreath(bool IsDraining);


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

	//Want to move all jump functionality to code
	//UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = StatEvent)
	//	void Jumped();

	//UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = StatEvent)
	//	void Landing();

	//SPRINTING-----------------------------
	void Sprint();

	UFUNCTION(BlueprintCallable)
		void StopSprinting();

private:
	void CheckStamina(float DeltaSeconds);
	void MoveBack();
	//void MoveBackReleased();
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

