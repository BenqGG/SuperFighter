// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SPPawnCPP.h"
#include "GameFramework/PlayerController.h"
#include "Net/UnrealNetwork.h"
#include "Math/UnrealMathUtility.h"
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

	// Called when the game starts or when spawned
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION(Server, reliable, WithValidation)
	void Server_SpawnPawn();

	UFUNCTION(Client, reliable)
		void Client_PostLogin();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = SPPC)
		APawn* Native_SpawnSPPawnBP(FVector Location, bool Server = false);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = SuperFighter)
		FVector2D AxisPosition();

};
