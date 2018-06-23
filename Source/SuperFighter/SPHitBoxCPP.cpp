// Fill out your copyright notice in the Description page of Project Settings.

#include "SuperFighter.h"
#include "SPHitBoxCPP.h"
#include "SPPawnCPP.h"


// Sets default values
ASPHitBoxCPP::ASPHitBoxCPP()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MainBody = CreateDefaultSubobject<USphereComponent>(TEXT("Hit_Box"));
	RootComponent = MainBody;
	WorkData.Active = false;
}

void ASPHitBoxCPP::SetHitbox(FSPHitBoxDetails l_details)
{
	if (HasAuthority()) {
		Details = l_details;
		MainBody->SetSphereRadius(Details.Position.Z);
		this->SetActorLocation(FVector(Details.Position.X, 0.0f, Details.Position.Y), false);
		GetWorldTimerManager().SetTimer(WorkData.ActivationTimer, this, &ASPHitBoxCPP::ActivateHitBox, Details.ActivationTime, false);
		MainBody->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
		
}

// Called when the game starts or when spawned
void ASPHitBoxCPP::BeginPlay()
{
	Super::BeginPlay();
}

void ASPHitBoxCPP::Follow()
{
	FVector OwnerPosition = Details.Owner->GetActorLocation();
	FVector FollowPosition;
	if(Details.Owner->FacingRight())
	FollowPosition.X = OwnerPosition.X + Details.FollowDistance.X;
	else
	FollowPosition.X = OwnerPosition.X - Details.FollowDistance.X;
	FollowPosition.Z = OwnerPosition.Z + Details.FollowDistance.Y;
	FollowPosition.Y = 0.0f;
	
	SetActorLocation(FollowPosition, false);
}

// Called every frame
void ASPHitBoxCPP::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
	if (Details.FollowPlayer) {
		Follow();
	}

}

void ASPHitBoxCPP::ActivateHitBox()
{
	if (HasAuthority()) {
		WorkData.Active = true;
		MainBody->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		GetWorldTimerManager().ClearTimer(WorkData.ActivationTimer);
		GetWorldTimerManager().SetTimer(WorkData.DestroyTimer, this, &ASPHitBoxCPP::DestroyHitBox, Details.DestroyTime, false);
	}
}

void ASPHitBoxCPP::DestroyHitBox()
{
	GetWorldTimerManager().ClearTimer(WorkData.DestroyTimer);
	AActor::Destroy(true, false);
}
