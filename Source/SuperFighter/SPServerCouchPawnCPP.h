// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Pawn.h"
#include "SPServerCouchPawnCPP.generated.h"

USTRUCT(BlueprintType)
struct FSPSCPData {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SuperFighter)
	int PlayersAmount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SuperFighter)
	TArray<AActor*> Players;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SuperFighter)
		float Step;
	FVector TargetPosition;
};

UCLASS()
class SUPERFIGHTER_API ASPServerCouchPawnCPP : public APawn
{
	GENERATED_BODY()

protected:

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SuperFighter)
		FSPSCPData Data;

public:
	// Sets default values for this pawn's properties
	ASPServerCouchPawnCPP();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = SuperFighter)
		void FindPlayers();

	UFUNCTION(BlueprintCallable, Category = SuperFighter)
		void SetPosition();

	void MoveCamera(float deltaTime);
};
