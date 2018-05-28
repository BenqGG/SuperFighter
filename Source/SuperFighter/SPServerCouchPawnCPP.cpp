// Fill out your copyright notice in the Description page of Project Settings.

#include "SuperFighter.h"
#include "SPServerCouchPawnCPP.h"


// Sets default values
ASPServerCouchPawnCPP::ASPServerCouchPawnCPP()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Data.PlayersAmount = 0;

}

// Called when the game starts or when spawned
void ASPServerCouchPawnCPP::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASPServerCouchPawnCPP::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	if (HasAuthority()) {
		if (Data.PlayersAmount < 2) {
			FindPlayers();
		}
		SetPosition();
	}

}

// Called to bind functionality to input
void ASPServerCouchPawnCPP::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);

}

void ASPServerCouchPawnCPP::SetPosition()
{
	if (HasAuthority()) {
		if (Data.PlayersAmount == 1) {
			SetActorLocation(Data.Players[0]->GetActorLocation(), false);
		}
		else if (Data.PlayersAmount > 1) {
			FVector Pos1 = Data.Players[0]->GetActorLocation();
			FVector Pos2 = Data.Players[1]->GetActorLocation();
			float absP1X, absP1Y, absP2X, absP2Y;
			float absDistanceX, absDistanceY;
			float middleX, middleY, distance;

			absP1X = Pos1.X;
			if (absP1X < 0.0f) absP1X *= -1.0f;
			absP1Y = Pos1.Z;
			if (absP1Y < 0.0f) absP1Y *= -1.0f;

			absP2X = Pos2.X;
			if (absP2X < 0.0f) absP2X *= -1.0f;
			absP2Y = Pos2.Z;
			if (absP2Y < 0.0f) absP2Y *= -1.0f;
			
			absDistanceX = absP1X - absP2X;
			if (absDistanceX < 0.0f) absDistanceX *= -1.0f;
			if (absDistanceX < 400.0f) absDistanceX = 400.0f;
			absDistanceY = (absP1Y - absP2Y ) * 1.7f;
			if (absDistanceY < 0.0f) absDistanceY *= -1.0f;



			middleX = Pos1.X - ((Pos1.X - Pos2.X) / 2.0f);
			middleY = Pos1.Z - ((Pos1.Z - Pos2.Z) / 2.0f);

			//if Horizontal distance is bigger than vertical
			if (absDistanceX >= absDistanceY ) {
				distance = Pos1.X - Pos2.X;
			}
			else {
				distance = (Pos1.Z - Pos2.Z);
			}

			if (distance < 0) distance *= -1.0f;
			distance = (distance / 2) + 300.0f;
			SetActorLocation(FVector(middleX, distance, middleY), false);
		}
	}
}

void ASPServerCouchPawnCPP::FindPlayers_Implementation()
{
}

