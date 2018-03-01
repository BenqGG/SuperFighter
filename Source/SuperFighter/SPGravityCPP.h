// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "SPGravityCPP.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGravityDelegate, const float, Pull);

UCLASS()
class SUPERFIGHTER_API ASPGravityCPP : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASPGravityCPP();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(BlueprintReadWrite, Category = Gravity)
		//Gracity force per second
		float FORCE_PER_SECOND;

	UPROPERTY(BlueprintReadOnly, Category = Gravity)
		//IS pulling
		float IsPulling;

	UPROPERTY(BlueprintReadWrite, Category = Gravity)
		//Gravity "pulls" objects (sends delegates) every x second
		float PULL_EVERY_X;

	//Current time delta from last pull
	float CurrentDelta;

	UFUNCTION(BlueprintCallable, Category = RawOre)
		//Should Pull Or No
		void SetGravityState(bool is_pulling);

	UFUNCTION(BlueprintCallable, Category = RawOre)
		//Update Gravity
		void UpdateGravity(float delta);

	UPROPERTY(BlueprintAssignable, Category = General)
		FGravityDelegate OnGravityPull;

};
