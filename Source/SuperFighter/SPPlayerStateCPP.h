// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/PlayerState.h"
#include "SPPlayerStateCPP.generated.h"

/**
 * 
 */
UCLASS()
class SUPERFIGHTER_API ASPPlayerStateCPP : public APlayerState
{
	GENERATED_BODY()
	// Sets default values for this actor's properties
		ASPPlayerStateCPP();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

	//Caled When Dc
	virtual void OnDeactivated() override;
	
};
