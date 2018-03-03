// Fill out your copyright notice in the Description page of Project Settings.

#include "SuperFighter.h"
#include "SPPlayerControllerCPP.h"



ASPPlayerControllerCPP::ASPPlayerControllerCPP()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void ASPPlayerControllerCPP::BeginPlay()
{
	Super::BeginPlay();
}

void ASPPlayerControllerCPP::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}
