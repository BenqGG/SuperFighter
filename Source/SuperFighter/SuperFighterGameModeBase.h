// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SPPlayerControllerCPP.h"
#include "GameFramework/GameModeBase.h"
#include "Net/UnrealNetwork.h"
#include  "Kismet/GameplayStatics.h"
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

	// Overriding the PostLogin function
	virtual void PostLogin(APlayerController* NewPlayer) override;
};
