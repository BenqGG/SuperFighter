// Fill out your copyright notice in the Description page of Project Settings.

#include "SuperFighter.h"
#include "SuperFighterGameModeBase.h"

ASuperFighterGameModeBase::ASuperFighterGameModeBase()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void ASuperFighterGameModeBase::BeginPlay()
{	
	Super::BeginPlay();
}

void ASuperFighterGameModeBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void ASuperFighterGameModeBase::PostLogin(APlayerController * NewPlayer)
{
	Super::PostLogin(NewPlayer);
	ASPPlayerControllerCPP *CastedNewPlayer = Cast<ASPPlayerControllerCPP>(NewPlayer);
	
	if (IsValid(CastedNewPlayer))
	{
		CastedNewPlayer->Client_PostLogin();
	}
}


