// Fill out your copyright notice in the Description page of Project Settings.

#include "SuperFighter.h"
#include "SPPlayerStateCPP.h"

ASPPlayerStateCPP::ASPPlayerStateCPP()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
}

void ASPPlayerStateCPP::BeginPlay()
{
	Super::BeginPlay();
}

void ASPPlayerStateCPP::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (HasAuthority() && ExactPing > 0.1f) {
		GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Yellow, "PING: " + FString::SanitizeFloat(ExactPing));
	}
	else if(Ping*4.0f > 0.1f) {
		GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Yellow, "PING: " + FString::SanitizeFloat(Ping*4.0f));
	}
}

void ASPPlayerStateCPP::OnDeactivated()
{
	Super::OnDeactivated();
	//if (HasAuthority()) {
		GEngine->AddOnScreenDebugMessage(-1, 4.0f, FColor::Red, "PLAYER DCED" );
	//}
}
