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
}

void ASPHitBoxCPP::SetHitbox(FSPHitBoxDetails l_details)
{
		Details = l_details;
		GetWorldTimerManager().SetTimer(DestroyTimer, this, &ASPHitBoxCPP::DestroyHitBox, Details.ExistTime, false);
		MainBody->SetSphereRadius(Details.Position.Y);
		SetActorLocation(FVector(Details.Position.X, 0, Details.Position.Z));
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

