// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/PlayerController.h"
#include "SPPlayerControllerCPP.generated.h"

/**
 * 
 */
UCLASS()
class SUPERFIGHTER_API ASPPlayerControllerCPP : public APlayerController
{
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties
	ASPPlayerControllerCPP();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;
};
