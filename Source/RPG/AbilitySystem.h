// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AbilitySystem.generated.h"

class ACharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RPG_API UAbilitySystem : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAbilitySystem();

	UPROPERTY(EditAnywhere)
		ACharacter* MyCharacter;

	//ABILITIES-----------------------------
	void Dash(); //Stag Dash
	void HighJump(); //crow ability
	void FindGrass();

	UFUNCTION(BlueprintCallable)
		void Rest();

	//Upgrades

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame


	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
