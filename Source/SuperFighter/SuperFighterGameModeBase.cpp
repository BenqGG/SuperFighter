// Fill out your copyright notice in the Description page of Project Settings.

#include "SuperFighter.h"
#include "SuperFighterGameModeBase.h"

ASuperFighterGameModeBase::ASuperFighterGameModeBase()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	WorkData.PlayersAmount = 0;
}

void ASuperFighterGameModeBase::BeginPlay()
{	
	Super::BeginPlay();
}

void ASuperFighterGameModeBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Yellow, "Players Amount: " + FString::FromInt(WorkData.PlayersAmount));
}

void ASuperFighterGameModeBase::PostLogin(APlayerController * NewPlayer)
{
	Super::PostLogin(NewPlayer);
	ASPPlayerControllerCPP *CastedNewPlayer = Cast<ASPPlayerControllerCPP>(NewPlayer);
	
	if (IsValid(CastedNewPlayer) && WorkData.PlayersAmount < 2)
	{
		WorkData.PlayersAmount++;
		CastedNewPlayer->Client_PostLogin();
	}
	else if(IsValid(CastedNewPlayer)) {
		CastedNewPlayer->Destroy();
	}
}


