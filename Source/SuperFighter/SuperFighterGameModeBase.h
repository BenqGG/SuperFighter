// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameModeBase.h"
#include "SuperFighterGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class SUPERFIGHTER_API ASuperFighterGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties
	ASuperFighterGameModeBase();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;
};
