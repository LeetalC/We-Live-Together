// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Quest.generated.h"

USTRUCT(BlueprintType)
struct FQuestInfos
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = Variables)
		FString QuestName;

	UPROPERTY(BlueprintReadWrite, Category = Variables)
		FString QuestDescription;

	UPROPERTY(BlueprintReadWrite, Category = Variables)
		float QuestReward;

	UPROPERTY(BlueprintReadWrite, Category = Variables)
		bool IsActive;

	UPROPERTY(BlueprintReadWrite, Category = Variables)
		bool IsComplete;

};
UCLASS()
class RPG_API AQuest : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AQuest();
	/*UFUNCTION(BlueprintCallable)
		void SetActive();
	UFUNCTION(BlueprintCallable)
		void SetInactive();*/
	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;



};
