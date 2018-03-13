// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Pawn.h"
#include "PaperFlipbookComponent.h"
#include "Net/UnrealNetwork.h"
#include "SPHitBoxCPP.h"
#include "SPPawnCPP.generated.h"

USTRUCT(BlueprintType)
struct FSPAnimationDetails {
	GENERATED_BODY()

		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector2D HitBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector2D CollisionBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UPaperFlipbook *Flipbook;
};

USTRUCT(BlueprintType)
struct FSPPawnAttributes {

	GENERATED_BODY()

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SuperFighter)
		float MoveSpeed = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SuperFighter)
		float JumpPower = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SuperFighter)
		int AirJumpAmount = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SuperFighter)
		float Friction = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SuperFighter)
		float AirFriction = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SuperFighter)
		float Gravity = 0;
};

USTRUCT(BlueprintType)
struct FSPPawnStates {

	GENERATED_BODY()

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SuperFighter)
		bool MOVE_RIGHT = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SuperFighter)
		bool MOVE_LEFT = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SuperFighter)
		bool JUMP = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SuperFighter)
		bool JUMP_LEFT_WALL = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SuperFighter)
		bool JUMP_RIGHT_WALL = false;
};

USTRUCT(BlueprintType)
struct FSPWorkData {

	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SuperFighter)
		int AirJumped;

		FTimerHandle JumpTimer;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SuperFighter)
		bool IsLocal;

		bool FacingRight;
};

USTRUCT(BlueprintType)
struct FSPStaticPawnAttributes {

	GENERATED_BODY()

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SuperFighter)
		//How much movement speed per movespeed attribute
		float MovementScale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SuperFighter)
		//How much movement speed per movespeed attribute
		float AirMovementScale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SuperFighter)
		float WallJumpYModifier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SuperFighter)
		float WallJumpXModifier;
};

UCLASS()
class SUPERFIGHTER_API ASPPawnCPP : public APawn
{
	GENERATED_BODY()

protected:

	FVector2D Forces;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = SuperFighter)
		FSPPawnAttributes Attributes;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = SuperFighter)
		FSPPawnStates States;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SuperFighter)
		FSPStaticPawnAttributes StaticAttributes;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = SuperFighter)
		FSPWorkData WorkData;
	UPROPERTY(ReplicatedUsing = RepNot_UpdatePosition, EditAnywhere, BlueprintReadWrite, Category = SuperFighter)
		FVector CurrentPosition;

	//Apply Forces working on player
	void ApplyForces(float DeltaTime);
	void CalculateMovement();

	//Return value of "value" giving its value is per second and we calc it per deltaTime
	float ValuePerSecond(float value, float deltaTime);

	UFUNCTION(BlueprintCallable, Category = SuperFighter)
		void SetAttributes(FSPPawnAttributes new_attributes);

	UFUNCTION(BlueprintCallable, Category = SuperFighter)
		void Move(bool right);

	UFUNCTION(BlueprintCallable, Category = SuperFighter)
		void StopMove();

	UFUNCTION(BlueprintCallable, Category = SuperFighter)
		void ChangeAnimation(FSPAnimationDetails details);

	void Friction(float DeltaTime);
	void Gravity(float DeltaTime);

public:

	// Sets default values for this pawn's properties
	ASPPawnCPP();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = SuperFighter)
		UBoxComponent *collision_box;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = SuperFighter)
		UBoxComponent *hit_box;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = SuperFighter)
		UPaperFlipbookComponent *animation;

	UFUNCTION(BlueprintCallable, Category = SuperFighter)
		void Jump();

	UFUNCTION(BlueprintCallable, Category = SuperFighter)
		void StopJump();

	UFUNCTION(BlueprintCallable, Category = SuperFighter)
		void Hit(float forceX, float forcey);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = SuperFighter)
		bool GroundUnderFeet() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = SuperFighter)
		bool GroundNextToFeet(bool right) const;

	void SetAsLocal() { WorkData.IsLocal = true; }

	UFUNCTION(BlueprintCallable, Category = SuperFighter)
	bool IsLocal() { return WorkData.IsLocal;  }

	UFUNCTION(BlueprintCallable, Category = SuperFighter)
	void CallEndViewTarget();

	UFUNCTION(BlueprintCallable, Category = SuperFighter)
		void Local_Move(float AxisX);

	UFUNCTION()
	void RepNot_UpdatePosition();

	UFUNCTION(Server, unreliable, WithValidation, BlueprintCallable, Category = SuperFighter)
		void Server_Move(float AxisX);

	UFUNCTION(Server, unreliable, WithValidation)
		//Server Will Only detect the jump that clients asks for
		void Server_Jump();

	UFUNCTION(Server, unreliable, WithValidation)
		//Server Will Only detect the jump that clients asks for
		void Server_StopJump();

	UFUNCTION(BlueprintCallable, Category = SuperFighter)
		void HitPunch(bool FromClient = false, FVector2D ClientAxisPosition = FVector2D(0, 0));

	UFUNCTION(Server, unreliable, WithValidation)
		void Server_HitPunch(FVector2D AxisPosition);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = SuperFighter)
		FVector2D AxisPosition();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = SuperFighter)
		void SpawnHitBox(FSPHitBoxDetails l_details);

	void HitPosition(FVector2D AxisPosition, FVector& Position, FVector& Force);
};
