// Fill out your copyright notice in the Description page of Project Settings.

#include "SuperFighter.h"
#include "SuperFighterGameModeBase.h"
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

void ASPPlayerControllerCPP::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	
	if (HasAuthority()) {
		ASuperFighterGameModeBase* gm;
		gm = Cast<ASuperFighterGameModeBase>(GetWorld()->GetAuthGameMode());
		gm->RemovePlayer();
	}
}

FVector2D ASPPlayerControllerCPP::AxisPosition_Implementation()
{
	return FVector2D();
}


APawn * ASPPlayerControllerCPP::Native_SpawnSPPawnBP_Implementation(FVector Location, bool Server = false)
{
	return nullptr;
}

void ASPPlayerControllerCPP::Client_PostLogin_Implementation()
{
	Server_SpawnPawn();
	if (HasAuthority()) {
		/*ASPPawnCPP *MyPawn = Cast<ASPPawnCPP>(GetPawn());
		if (IsValid(MyPawn)) {
			MyPawn->SetAsLocal();
		}*/
	}
}

void ASPPlayerControllerCPP::Server_SpawnPawn_Implementation()
{
	ASuperFighterGameModeBase* gm;
	FVector location;
	gm = Cast<ASuperFighterGameModeBase>(GetWorld()->GetAuthGameMode());
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

	APawn* SpawnedPawn;
	if (gm->PlayersAmount() > 1) {
		location = gm->FindPlayerStart(this, "Uno")->GetActorLocation();
		SpawnedPawn = Native_SpawnSPPawnBP(location);
	}
	
	else {
		location = gm->FindPlayerStart(this, "Dos")->GetActorLocation();
		SpawnedPawn = Native_SpawnSPPawnBP(location, true);
	}
	

	if (IsValid(SpawnedPawn)) {
		Possess(SpawnedPawn);
	}
}

bool ASPPlayerControllerCPP::Server_SpawnPawn_Validate()
{
	return true;
}
