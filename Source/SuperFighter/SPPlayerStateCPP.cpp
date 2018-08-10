// Fill out your copyright notice in the Description page of Project Settings.

#include "SuperFighter.h"
#include "SPPlayerStateCPP.h"

ASPPlayerStateCPP::ASPPlayerStateCPP(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PlayerInfo.Stocks = 0;
	PlayerInfo.Lost = false;
	
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

	if (PlayerInfo.Lost) {
		CheckEndMatch();
	}
}

void ASPPlayerStateCPP::OnDeactivated()
{
	Super::OnDeactivated();
	//if (HasAuthority()) {
		GEngine->AddOnScreenDebugMessage(-1, 4.0f, FColor::Red, "PLAYER DCED" );
	//}
}

void ASPPlayerStateCPP::LooseStock()
{
	if (HasAuthority()) {
		PlayerInfo.Stocks--;
		if (PlayerInfo.Stocks < 1) {
			PlayerInfo.Lost = true;
		}
	}
}

void ASPPlayerStateCPP::CheckEndMatch()
{
	
	APlayerController* MyController = Cast<APlayerController>(GetOwner());
	
	if (IsValid(MyController)) {
		if (MyController->IsLocalController()) {
			EndMatch(false);
		}
		else {
			EndMatch(true);
		}
	}
	else {
		EndMatch(true);
	}
}

void ASPPlayerStateCPP::EndMatch_Implementation(bool won)
{

}

void ASPPlayerStateCPP::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ASPPlayerStateCPP, PlayerInfo);
}
