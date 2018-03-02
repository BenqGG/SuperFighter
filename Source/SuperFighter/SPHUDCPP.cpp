// Fill out your copyright notice in the Description page of Project Settings.

#include "SuperFighter.h"
#include "SPHUDCPP.h"
#include "Blueprint/UserWidget.h"

ASPHUDCPP::ASPHUDCPP()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void ASPHUDCPP::BeginPlay()
{
	Super::BeginPlay();

	if (wMainHUD) // Check if the Asset is assigned in the blueprint.
	{
		// Create the widget and store it.
		MainHUD = CreateWidget<UUserWidget>(UGameplayStatics::GetPlayerController(GetWorld(), 0), wMainHUD);

		// now you can use the widget directly since you have a referance for it.
		// Extra check to  make sure the pointer holds the widget.
		if (MainHUD)
		{
			//let add it to the view port
			MainHUD->AddToViewport();
		}
	}
}

void ASPHUDCPP::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}
