// Fill out your copyright notice in the Description page of Project Settings.

#include "SuperFighter.h"
#include "SPGravityCPP.h"


// Sets default values
ASPGravityCPP::ASPGravityCPP()
{
	if (HasAuthority()) {
		// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
		PrimaryActorTick.bCanEverTick = true;
	}
	else {
		PrimaryActorTick.bCanEverTick = false;
	}

	CurrentDelta = 0;
	FORCE_PER_SECOND = 0;
	PULL_EVERY_X = 0;
	IsPulling = false;
}

// Called when the game starts or when spawned
void ASPGravityCPP::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ASPGravityCPP::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateGravity(DeltaTime);
}

//Should Pull Or No
void ASPGravityCPP::SetGravityState(bool is_pulling)
{
	if (HasAuthority()) {
		IsPulling = is_pulling;
		CurrentDelta = 0;
	}
}

//Update Gravity
void ASPGravityCPP::UpdateGravity(float delta)
{
	if (HasAuthority()) {

		float current_pull = 0;
		if (IsPulling) {
			CurrentDelta += delta;
			if (CurrentDelta >= PULL_EVERY_X) {
				while (CurrentDelta >= PULL_EVERY_X) {
					CurrentDelta -= PULL_EVERY_X;
					current_pull += FORCE_PER_SECOND * (PULL_EVERY_X / 1.0f);
				}
				OnGravityPull.Broadcast(current_pull);
			}
		}
	}
}

