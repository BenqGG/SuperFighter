// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/PlayerState.h"
#include "Runtime/Core/Public/Templates/Function.h"
#include "Net/UnrealNetwork.h"
#include "SPPlayerStateCPP.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType)
struct FSPPlayerInfo {
	GENERATED_BODY()

		UPROPERTY()
		int	Stocks;
		UPROPERTY()
		bool Lost;
};

UCLASS()
class SUPERFIGHTER_API ASPPlayerStateCPP : public APlayerState
{

protected:
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, Category = SuperFighter)
	FSPPlayerInfo PlayerInfo;

public:
	GENERATED_BODY()
	// Sets default values for this actor's properties
		ASPPlayerStateCPP(const FObjectInitializer& ObjectInitializer);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

	//Caled When Dc
	virtual void OnDeactivated() override;

	UFUNCTION(BlueprintCallable, Category = SuperFighter)
	void SetStocks(int value) { if (HasAuthority() )PlayerInfo.Stocks = value; }

	UFUNCTION(BlueprintCallable, Category = SuperFighter)
		void LooseStock();

	UFUNCTION(BlueprintCallable, Category = SuperFighter)
		int GetStocks() { return PlayerInfo.Stocks; };

	void CheckEndMatch();

	UFUNCTION(BlueprintNativeEvent, Category = SuperFighter)
	void EndMatch(bool won);
};
