// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Net/UnrealNetwork.h"
#include "DrawDebugHelpers.h"
#include "PaperFlipbookComponent.h"
#include "SPHitBoxCPP.generated.h"

UENUM(BlueprintType)
enum class HitType : uint8 
{ 
	HT_Pawn UMETA(DisplayName = "Pawn"), 
	HT_Missile UMETA(DisplayName = "Missile")
};

USTRUCT(BlueprintType)
struct FSPHitBoxDetails {

	GENERATED_BODY()

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SuperFighter)
		//X Stand For X, Y Stands For Z(Z but Y), Z stands for radius
		FVector Position;
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SuperFighter)
		//X is x multiplier , Y is y(Z) multiplier and Z is force to be multiplied
		FVector Force;
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SuperFighter)
		//How long from birth to activation
		float ActivationTime;
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SuperFighter)
		//How long from activation to destroy
		float DestroyTime;
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SuperFighter)
		bool FriendlyFire;
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SuperFighter)
		bool MultiHit;
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SuperFighter)
		class ASPPawnCPP *Owner;
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SuperFighter)
		float Damage;
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SuperFighter)
		float HitStun;
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SuperFighter)
		bool FollowPlayer;
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SuperFighter)
		FVector2D FollowDistance;
};

USTRUCT(BlueprintType)
struct FSPHitBoxWorkData {

	GENERATED_BODY()

	FTimerHandle ActivationTimer;
	FTimerHandle DestroyTimer;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SuperFighter)
	bool Active;
};

USTRUCT(BlueprintType)
struct FSPMissileDetails {

	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SuperFighter)
	bool Missile;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SuperFighter)
	float ExplodeTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SuperFighter)
	FVector Trajectory;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SuperFighter)
	UPaperFlipbook *Prepare;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SuperFighter)
	UPaperFlipbook *Launch;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SuperFighter)
	UPaperFlipbook *Hit;


	FTimerHandle Timer;
	bool Launched;
};

UCLASS()
class SUPERFIGHTER_API ASPHitBoxCPP : public AActor
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SuperFighter)
	FSPHitBoxDetails Details;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SuperFighter)
	FSPHitBoxWorkData WorkData;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SuperFighter)
	USphereComponent* MainBody;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SuperFighter)
	UPaperFlipbookComponent* Animation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SuperFighter)
		FSPMissileDetails MissileDetails;
	
		
public:	
	// Sets default values for this actor's properties
	ASPHitBoxCPP();

	UFUNCTION(BlueprintCallable, Category = SuperFighter)
	void SetHitbox(FSPHitBoxDetails l_details);
		
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void Follow();
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	UFUNCTION(BlueprintCallable, Category = SuperFighter)
	void ActivateHitBox();
	
	UFUNCTION(BlueprintCallable, Category = SuperFighter)
	void DestroyHitBox();

	void UpdateMissile(float DeltaTime);

	UFUNCTION(BlueprintCallable, Category = SuperFighter)
		void SetMissile();

	UFUNCTION(BlueprintCallable, Category = SuperFighter)
		void ExplodeOnHit();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = SuperFighter)
		void HideAnimation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = SuperFighter)
		void ShowAnimation();
};
