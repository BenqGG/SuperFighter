// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/HUD.h"
#include "SPHUDCPP.generated.h"

/**
 * 
 */
UCLASS()
class SUPERFIGHTER_API ASPHUDCPP : public AHUD
{
	GENERATED_BODY()
	
	

public:
	// Sets default values for this actor's properties
	ASPHUDCPP();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;
	
	// Reference UMG Asset in the Editor
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
		TSubclassOf<class UUserWidget> wMainHUD;

	// Variable to hold the widget After Creating it.
	UUserWidget* MainHUD;

};
