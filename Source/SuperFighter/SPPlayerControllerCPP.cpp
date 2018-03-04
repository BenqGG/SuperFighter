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


APawn * ASPPlayerControllerCPP::Native_SpawnSPPawnBP_Implementation(FVector Location)
{
	return nullptr;
}

void ASPPlayerControllerCPP::Client_PostLogin_Implementation()
{
	Server_SpawnPawn();
	if (HasAuthority()) {
		ASPPawnCPP *MyPawn = Cast<ASPPawnCPP>(GetPawn());
		if (IsValid(MyPawn)) {
			MyPawn->SetAsLocal();
		}
	}
}

void ASPPlayerControllerCPP::Server_SpawnPawn_Implementation()
{
	AGameModeBase* gm;
	FVector location;
	gm = GetWorld()->GetAuthGameMode();
	switch (FMath::RandRange((int)0, (int)3)) {
	case 0:
		location = gm->FindPlayerStart(this, "Uno")->GetActorLocation();
		break;

	case 1:
		location = gm->FindPlayerStart(this, "Dos")->GetActorLocation();
		break;

	case 2:
		location = gm->FindPlayerStart(this, "Tres")->GetActorLocation();
		break;

	case 3:
		location = gm->FindPlayerStart(this, "Quatro")->GetActorLocation();
		break;

	default:
		location = gm->FindPlayerStart(this, "Uno")->GetActorLocation();
		break;
	}

	APawn* SpawnedPawn = Native_SpawnSPPawnBP(location);
	if (IsValid(SpawnedPawn)) {
		Possess(SpawnedPawn);
	}
}

bool ASPPlayerControllerCPP::Server_SpawnPawn_Validate()
{
	return true;
}
