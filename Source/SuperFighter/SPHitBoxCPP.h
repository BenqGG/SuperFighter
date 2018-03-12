// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "SPHitBoxCPP.generated.h"

USTRUCT(BlueprintType)
struct FSPHitBoxDetails {

	GENERATED_BODY()

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SuperFighter)
		//X Stand For X, Z Stands For Y(Z but Y), Y stands for radius
		FVector Position;
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SuperFighter)
		FVector Force;
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SuperFighter)
		float ExistTime;
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SuperFighter)
		bool FriendlyFire;
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SuperFighter)
		AActor* Owner;
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SuperFighter)
		float HitStun;
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SuperFighter)
		bool MultiHit;
};

UCLASS()
class SUPERFIGHTER_API ASPHitBoxCPP : public AActor
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SuperFighter)
		FSPHitBoxDetails Details;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SuperFighter)
	USphereComponent* MainBody;
	FTimerHandle DestroyTimer;
		
public:	
	// Sets default values for this actor's properties
	ASPHitBoxCPP();

	UFUNCTION(BlueprintCallable, Category = SuperFighter)
	void SetHitbox(FSPHitBoxDetails l_details);
		
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;
	
	UFUNCTION(BlueprintCallable, Category = SuperFighter)
	void DestroyHitBox();
};
