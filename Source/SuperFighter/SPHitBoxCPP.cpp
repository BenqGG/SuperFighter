// Fill out your copyright notice in the Description page of Project Settings.

#include "SuperFighter.h"
#include "SPHitBoxCPP.h"


// Sets default values
ASPHitBoxCPP::ASPHitBoxCPP()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	MainBody = CreateDefaultSubobject<USphereComponent>(TEXT("Hit_Box"));
	RootComponent = MainBody;
	Active = false;
}

void ASPHitBoxCPP::SetHitbox(FSPHitBoxDetails l_details)
{
	Details = l_details;
	MainBody->SetSphereRadius(Details.Position.Y);
	GetWorldTimerManager().SetTimer(DestroyTimer, this, &ASPHitBoxCPP::DestroyHitBox, Details.ExistTime, false);
	Active = true;
	MainBody->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

// Called when the game starts or when spawned
void ASPHitBoxCPP::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ASPHitBoxCPP::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

void ASPHitBoxCPP::DestroyHitBox()
{
	GetWorldTimerManager().ClearTimer(DestroyTimer);
	AActor::Destroy(true, false);
}

void ASPHitBoxCPP::HitActor(AActor * HitActor, HitType Type)
{
	if (HasAuthority()) {
		if (Details.FriendlyFire || HitActor != Details.Owner) {
			if (Active) {
				switch (Type) {
				case HitType::HT_Pawn:
					HitPawn(HitActor);
					if (!Details.MultiHit) DestroyHitBox();
					break;

				case HitType::HT_Missile:
					break;

				default:
					break;
				}
			}
		}
	}
}

void ASPHitBoxCPP::HitPawn_Implementation(AActor * HitActor)
{
}
