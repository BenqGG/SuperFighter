// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Pawn.h"
#include "Runtime/Core/Public/Templates/Function.h"
#include "PaperFlipbookComponent.h"
#include "Net/UnrealNetwork.h"
#include <cmath>
#include "SPHitBoxCPP.h"
#include "SPPawnCPP.generated.h"

DECLARE_DYNAMIC_DELEGATE(FActionFunction);

class ASPPawnCPP;
typedef  void (ASPPawnCPP::*PawnActions)();
#define CALL_MEMBER_FN(object,ptrToMember)  ((object).*(ptrToMember))

USTRUCT(BlueprintType)
struct FSPPanActions {
	GENERATED_BODY()

	float delay; 	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SuperFighter)
	FActionFunction DelayAction;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SuperFighter)
	FActionFunction Move;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SuperFighter)
	FActionFunction StopMove;
	
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SuperFighter)
	FActionFunction Jump;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SuperFighter)
	FActionFunction StopJump;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SuperFighter)
	FActionFunction TouchGround;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SuperFighter)
	FActionFunction LeaveGround;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SuperFighter)
	FActionFunction Defence;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SuperFighter)
	FActionFunction ReleaseDefence;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SuperFighter)
	FActionFunction Dash;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SuperFighter)
	FActionFunction AirDash;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SuperFighter)
	FActionFunction SpotDodge;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SuperFighter)
	FActionFunction LightAttack;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SuperFighter)
	FActionFunction UpperLightAttack;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SuperFighter)
	FActionFunction DownLightAttack;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SuperFighter)
	FActionFunction AirLightAttack;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SuperFighter)
	FActionFunction AirUpperLightAttack;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SuperFighter)
	FActionFunction AirDownLightAttack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SuperFighter)
		FActionFunction RealeaseStrongAttack;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SuperFighter)
	FActionFunction StrongAttack;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SuperFighter)
		FActionFunction SideStrongAttack;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SuperFighter)
		FActionFunction UpperStrongAttack;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SuperFighter)
		FActionFunction DownStrongAttack;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SuperFighter)
		FActionFunction AirStrongAttack;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SuperFighter)
		FActionFunction AirSideStrongAttack;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SuperFighter)
		FActionFunction AirUpperStrongAttack;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SuperFighter)
		FActionFunction AirDownStrongAttack;
};

USTRUCT(BlueprintType)
struct FSPKeyStates {
	GENERATED_BODY()

	bool LEFT_KEY;
	bool RIGHT_KEY;
	bool UP_KEY;
	bool DOWN_KEY;

	bool LATTACK_KEY;
	bool SATTACK_KEY;
	bool JUMP_KEY;
	bool DEFENCE_KEY;
};

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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SuperFighter)
		//How much percent are injures lowered
		float Tenacity = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SuperFighter)
		//How Much damage defence can get
		float Defence = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SuperFighter)
		//Dash Speed
		float Dash = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SuperFighter)
		//Dash Time 
		float DashTime = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SuperFighter)
		//Spot Dodge Time 
		float SpotDodgeTime = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SuperFighter)
		//Jump Time 
		float JumpTime = 0;
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SuperFighter)
		bool ON_GROUND = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SuperFighter)
		//Character can not do anything from player commands while busy
		bool BUSY = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SuperFighter)
		//IF Can Move or no
		bool CAN_MOVE = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SuperFighter)
		//IF Can jump or no
		bool CAN_JUMP = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SuperFighter)
		bool CAN_LIGHT_ATTACK = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SuperFighter)
		bool CAN_STRONG_ATTACK = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SuperFighter)
		bool CAN_DEFENCE = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SuperFighter)
		bool CAN_DASH = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SuperFighter)
		bool DEFENCE = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SuperFighter)
		bool STRONG_ATTACK = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SuperFighter)
		bool LIGHT_ATTACK = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SuperFighter)
		bool DASH = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SuperFighter)
		bool SIDE_DASH = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SuperFighter)
		bool UP_DASH = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SuperFighter)
		bool DOWN_DASH = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SuperFighter)
		bool SPOT_DODGE = false;
};

//Beacuse server sometimes fall to send right dash info (side_dash/down_dash... etc) we need to store that info localy for client
USTRUCT(BlueprintType)
struct FSPClientDash {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SuperFighter)
		bool SIDE_DASH = false;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SuperFighter)
		bool UP_DASH = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SuperFighter)
		bool DOWN_DASH = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SuperFighter)
		bool SPOT_DODGE = false;
};

USTRUCT(BlueprintType)
struct FSPWorkData {

	GENERATED_BODY()

		FTimerHandle JumpTimer;
		FTimerHandle LightAttackTimer;
		FTimerHandle StrongAttackTimer;
		FTimerHandle DashTimer;
		FTimerHandle ClientTimer;

		FVector PossitionError;
		float HitStun = 0.0f;
		//+1 every 0.1 second
		int StrongAttackMeter = 0;
		bool WasHit = false;
		FVector HitForce;
		bool AddingForce = false;
		FVector AddForce;
		int AirJumped;
		bool IsLocal;
		bool FacingRight;
		float CurrentDefence = 0.0f;
		//More injuried you are the further you fly after getting hit and how long hit stun last, 
		//(SOME CHAPMIONS MAY USED TO SOME ADDITIONAL THINGS)
		//You can not add to hit stun (you can only hit stun unstun enemy, if he is already hit stun then hit stun wont replenish
		int Injuries = 0;
		float DefenceDelta = 0.0f;

		bool CanJumpAgain = true;
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SuperFighter)
	FSPPanActions Actions;
	FTimerHandle DelayTimer;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SuperFighter)
	FVector2D Forces;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = SuperFighter)
		FSPPawnAttributes Attributes;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SuperFighter)
		FSPPawnStates States;
	UPROPERTY(ReplicatedUsing = RepNot_UpdateStates, EditAnywhere, BlueprintReadWrite, Category = SuperFighter)
		FSPPawnStates ClientStates;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = SuperFighter)
		FSPStaticPawnAttributes StaticAttributes;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SuperFighter)
		FSPWorkData WorkData;

	UPROPERTY(ReplicatedUsing = RepNot_UpdateClientForces, EditAnywhere, BlueprintReadWrite, Category = SuperFighter)
		FVector2D Client_Forces;
	UPROPERTY(ReplicatedUsing = RepNot_UpdatePosition, EditAnywhere, BlueprintReadWrite, Category = SuperFighter)
		FVector ClientPosition;
	UPROPERTY(ReplicatedUsing = RepNot_UpdateHitStun, EditAnywhere, BlueprintReadWrite, Category = SuperFighter)
		float ClientHitStun;
	UPROPERTY(ReplicatedUsing = RepNot_UpdateStrongAttackMeter, EditAnywhere, BlueprintReadWrite, Category = SuperFighter)
		int ClientStrongAttackMeter;
	UPROPERTY(ReplicatedUsing = RepNot_UpdateAirJumped, EditAnywhere, BlueprintReadWrite, Category = SuperFighter)
		int ClientAirJumped;
	UPROPERTY(ReplicatedUsing = RepNot_UpdateCurrentDefence, EditAnywhere, BlueprintReadWrite, Category = SuperFighter)
		float ClientCurrentDefence;
	UPROPERTY(ReplicatedUsing = RepNot_UpdateInjuries, EditAnywhere, BlueprintReadWrite, Category = SuperFighter)
		int ClientInjuries;
	UPROPERTY(ReplicatedUsing = RepNot_UpdateAnimation, EditAnywhere, BlueprintReadWrite, Category = SuperFighter)
		int ClientAnimation;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = SuperFighter)
		FSPClientDash ClientDash;

	FSPKeyStates KeyStates;
	FSPKeyStates LastKeyStates;

	//Apply Forces working on player
	void ApplyForces(float DeltaTime);
	void CalculateMovement(float DeltaTime);
	void CheckKeyStates();

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

	UFUNCTION(BlueprintCallable, Category = SuperFighter)
		void ChangeMovementSpeed(float speed);

	UFUNCTION(BlueprintCallable, Category = SuperFighter)
		void AddForce(FVector force);

	void Friction(float DeltaTime);
	void Gravity(float DeltaTime);

	void FixPossitionError();

	void SetUpDefence();
	void ClearDefence();
	void UseDefence();
	void ReplenishDefence();

	void SetUpDash();
	void StopDash();
	void DashColdown();

	UFUNCTION(BlueprintCallable, Category = SuperFighter)
	void StartLightAttack(float time);
	void EndLightAttack();

	void SetUpStrongAttack();
	void ClearStrongAttack();
	void UpgradeStrongAttackMeter();

	void ClearStatesWhileHit();

	void ManageSyncFrames(float DeltaTime);

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
		void MakeBusy();

	UFUNCTION(BlueprintCallable, Category = SuperFighter)
		void UnBusy();

	UFUNCTION(BlueprintCallable, Category = SuperFighter)
		void SetCanJump(bool can) { if (HasAuthority() && States.CAN_JUMP != can) States.CAN_JUMP = can; };

	UFUNCTION(BlueprintCallable, Category = SuperFighter)
		void SetCanMove(bool can) { if (HasAuthority() && States.CAN_MOVE != can) States.CAN_MOVE = can; };

	UFUNCTION(BlueprintCallable, Category = SuperFighter)
		void SetCanLightAttack(bool can) { if (HasAuthority() && States.CAN_LIGHT_ATTACK != can) States.CAN_LIGHT_ATTACK = can; };

	UFUNCTION(BlueprintCallable, Category = SuperFighter)
		void SetCanStrongAttack(bool can) { if(HasAuthority() && States.CAN_STRONG_ATTACK != can) States.CAN_STRONG_ATTACK = can; };

	UFUNCTION(BlueprintCallable, Category = SuperFighter)
		void SetCanDash(bool can) { if (HasAuthority() && States.CAN_DASH != can) States.CAN_DASH = can; };

	UFUNCTION(BlueprintCallable, Category = SuperFighter)
		void StopJump();

	UFUNCTION(BlueprintCallable, Category = SuperFighter)
		void LightAttack(int index = 0/*0 - check, 1 - normal, 2 - upper, 3 - down*/);

	UFUNCTION(Server, unreliable, WithValidation)
		//Server Will Only detect the jump that clients asks for
		void Server_LightAttack();

	UFUNCTION(Server, unreliable, WithValidation)
		//Server Will Only detect the jump that clients asks for
		void Server_UpperLightAttack();

	UFUNCTION(Server, unreliable, WithValidation)
		//Server Will Only detect the jump that clients asks for
		void Server_DownLightAttack();

	UFUNCTION(BlueprintCallable, Category = SuperFighter)
		void StrongAttack(int index = 0/*0 - check, 1 - strong attack, 2 - side , 3 - upper, 4 - down*/);

	UFUNCTION(Server, unreliable, WithValidation)
		//Server Will Only detect the jump that clients asks for
		void Server_StrongAttack();

	UFUNCTION(Server, unreliable, WithValidation)
		//Server Will Only detect the jump that clients asks for
		void Server_SideStrongAttack();

	UFUNCTION(Server, unreliable, WithValidation)
		//Server Will Only detect the jump that clients asks for
		void Server_UpperStrongAttack();

	UFUNCTION(Server, unreliable, WithValidation)
		//Server Will Only detect the jump that clients asks for
		void Server_DownStrongAttack();

	UFUNCTION(BlueprintCallable, Category = SuperFighter)
		void ReleaseStrongAttack();

	UFUNCTION(Server, unreliable, WithValidation)
		//Server Will Only detect the jump that clients asks for
		void Server_ReleaseStrongAttack();

	UFUNCTION(BlueprintCallable, Category = SuperFighter)
		void Defence(int index = 0/*0 - check, 1 - defend, 2 - sidedash, 3 - updash, 4 - downdash/spotdodge*/);

	UFUNCTION(BlueprintCallable, Category = SuperFighter)
		void SetClientAnimation(int index);

	UFUNCTION(Server, unreliable, WithValidation)
		//Server Will Only detect the jump that clients asks for
		void Server_Defence();

	UFUNCTION(Server, unreliable, WithValidation)
		//Server Will Only detect the jump that clients asks for
		void Server_SideDash();

	UFUNCTION(Server, unreliable, WithValidation)
		//Server Will Only detect the jump that clients asks for
		void Server_UpDash();

	UFUNCTION(Server, unreliable, WithValidation)
		//Server Will Only detect the jump that clients asks for
		void Server_DownDash();

	UFUNCTION(BlueprintCallable, Category = SuperFighter)
		void ReleaseDefence();

	UFUNCTION(Server, reliable, WithValidation)
		//Server Will Only detect the jump that clients asks for
		void Server_ReleaseDefence();

	UFUNCTION(BlueprintCallable, Category = SuperFighter)
		void Hit(float forceX, float forcey);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = SuperFighter)
		bool GroundUnderFeet() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = SuperFighter)
		bool GroundNextToFeet(bool right) const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = SuperFighter)
		void DrawStunMeter(float Radius);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = SuperFighter)
		void PlayClientAnimation(int index);

	void SetAsLocal() { WorkData.IsLocal = true; }

	void StopDashForces();

	UFUNCTION(BlueprintCallable, Category = SuperFighter)
	bool IsLocal() { return WorkData.IsLocal;  }

	UFUNCTION(BlueprintCallable, Category = SuperFighter)
	void CallEndViewTarget();

	UFUNCTION(BlueprintCallable, Category = SuperFighter)
		void Local_Move(float AxisX);

	UFUNCTION()
	void RepNot_UpdatePosition();
	UFUNCTION()
	void RepNot_UpdateHitStun();
	UFUNCTION()
	void RepNot_UpdateClientForces();
	UFUNCTION()
	void RepNot_UpdateStates();
	UFUNCTION()
	void RepNot_UpdateStrongAttackMeter();
	UFUNCTION()
	void RepNot_UpdateAirJumped();
	UFUNCTION()
	void RepNot_UpdateCurrentDefence();
	UFUNCTION()
		void RepNot_UpdateInjuries();
	UFUNCTION()
		void RepNot_UpdateAnimation();

	UFUNCTION(Server, unreliable, WithValidation, BlueprintCallable, Category = SuperFighter)
		void Server_Move(float AxisX);

	UFUNCTION(Server, unreliable, WithValidation, BlueprintCallable, Category = SuperFighter)
		void Server_MoveLeft();
		
	UFUNCTION(Server, unreliable, WithValidation, BlueprintCallable, Category = SuperFighter)
		void Server_MoveRight();

	UFUNCTION(Server, reliable, WithValidation, BlueprintCallable, Category = SuperFighter)
		void Server_StopMove();

	UFUNCTION(Server, reliable, WithValidation)
		//Server Will Only detect the jump that clients asks for
		void Server_Jump();

	UFUNCTION(Server, reliable, WithValidation)
		//Server Will Only detect the jump that clients asks for
		void Server_StopJump();

	UFUNCTION(BlueprintCallable, Category = SuperFighter)
		void HitPunch(bool FromClient = false, FVector2D ClientAxisPosition = FVector2D(0, 0));

	UFUNCTION(BlueprintCallable, Category = SuperFighter)
		void ManageStunState(float DeltaTime);
	UFUNCTION(BlueprintCallable, Category = SuperFighter)
		void ManageDefence(float DeltaTime);

	UFUNCTION(Server, unreliable, WithValidation)
		void Server_HitPunch(FVector2D AxisPosition);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = SuperFighter)
		FVector2D AxisPosition();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = SuperFighter)
		void SpawnHitBox(FSPHitBoxDetails l_details);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = SuperFighter)
		void DodgeBlink(bool start);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = SuperFighter)
		void DrawDefence();

	void HitPosition(FVector2D AxisPosition, FVector& Position, FVector& Force);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = SuperFighter)
		//Called After Pawn Spawn as action to call idle state of character
		void SetUpIdle();

	UFUNCTION(BlueprintCallable, Category = SuperFighter)
		//Must be called everytime we change anything here
		void ResetActions(float delay_delta);

	UFUNCTION(NetMulticast, reliable, WithValidation, Category = SuperFighter)
		void CallDelayAction();

	UFUNCTION(BlueprintCallable, Category = SuperFighter)
		void CallTouchGround();

	UFUNCTION(BlueprintCallable, Category = SuperFighter)
		void CallLeaveGround();

	UFUNCTION(NetMulticast, reliable, WithValidation, Category = SuperFighter)
		void Client_Dash(FVector2D n_Position, int index /*0 - side dash, 1 - up dash, 2 - down dash, 3 - spot dodge*/);

	UFUNCTION(NetMulticast, reliable, WithValidation, Category = SuperFighter)
		void Client_Move(FVector2D n_Position, int index /*0 - right, 1 - left*/);

	UFUNCTION(NetMulticast, reliable, WithValidation, Category = SuperFighter)
		void Client_StopMove(FVector2D n_Position);

	UFUNCTION(NetMulticast, reliable, WithValidation, Category = SuperFighter)
		void Client_Jump(FVector2D n_Position, int index/*0 - jump, 1 - right wall, 2 -left wall*/);

	UFUNCTION(NetMulticast, reliable, WithValidation, Category = SuperFighter)
		void Client_StopJump(FVector2D n_Position);

	UFUNCTION(NetMulticast, reliable, WithValidation, Category = SuperFighter)
		void Client_Defence(FVector2D n_Position);

	UFUNCTION(NetMulticast, reliable, WithValidation, Category = SuperFighter)
		void Client_StopDefence(FVector2D n_Position);

	UFUNCTION(NetMulticast, reliable, WithValidation, Category = SuperFighter)
		void Client_LightAttack(FVector2D n_Position, int index /*0 - normal, 1 - up, 2 - down, 3 - normal air, 4 - up airm 5 - down air*/ );
		
	UFUNCTION(NetMulticast, reliable, WithValidation, Category = SuperFighter)
		void Client_StrongAttack(FVector2D n_Position, int index /*0 - normal, 1 - side, 2 - up, 3 - down, 4 - air normal, 5 - air side, 6 - air up, 7 - air down */);

	UFUNCTION(NetMulticast, reliable, WithValidation, Category = SuperFighter)
		void Client_StopStrongAttack(FVector2D n_Position, int StrongAttackMeter);

	UFUNCTION(NetMulticast, reliable, WithValidation, Category = SuperFighter)
		void Client_CallDelayAction(FVector2D n_Position);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = SuperFighter)
		void GetHit(float hitstun, float damage, FVector knockback);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = SuperFighter)
		void EndStun();

	void CallActionFunction(ASPPawnCPP& o, PawnActions p) {
		CALL_MEMBER_FN(o, p)();
	}

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = SuperFighter)
		void ChangeAnimationRotation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = SuperFighter)
		void DashEnd();

	UFUNCTION(BlueprintCallable, Category = SuperFighter)
	bool FacingRight();

	UFUNCTION(BlueprintCallable, Category = SuperFighter)
		float StrongAttackMeter();

	UFUNCTION(BlueprintCallable, Category = SuperFighter)
		float CurrentDefence();

	FVector2D GetSendPosition();

	UFUNCTION(BlueprintCallable, Category = SuperFighter)
		//For Client we mainly use ClientStates because we Call events based on comparing States to Client states
		//Then in those Evenets we want to check most current states for server those are States
		//And for client those are ClientStates and after we call the events we finaly do states=clientstates
		FSPPawnStates CurrentStates() { if (true) return States; return ClientStates; };

	UFUNCTION(BlueprintCallable, Category = SuperFighter)
	bool IsStun();
	bool CanMove();
	bool CanStopMove();
	bool CanDelayAction();
	bool CanJump();
	bool CanStopJump();
	bool CanLightAttack();
	bool CanStrongAttack();
	bool CanReleaseStrongAttack();
	bool CanDefence();
	bool CanReleaseDefence();
	bool CanDash();

	UFUNCTION(BlueprintCallable, Category = SuperFighter)
		void SetLeftKey(bool state) { KeyStates.LEFT_KEY = state; };
	UFUNCTION(BlueprintCallable, Category = SuperFighter)
		void SetRightKey(bool state) { KeyStates.RIGHT_KEY = state; };
	UFUNCTION(BlueprintCallable, Category = SuperFighter)
		void SetUpKey(bool state) { KeyStates.UP_KEY = state; };
	UFUNCTION(BlueprintCallable, Category = SuperFighter)
		void SetDownKey(bool state) { KeyStates.DOWN_KEY = state; };

	UFUNCTION(BlueprintCallable, Category = SuperFighter)
		void SetLAttackKey(bool state) { KeyStates.LATTACK_KEY = state; };
	UFUNCTION(BlueprintCallable, Category = SuperFighter)
		void SetSAttackKey(bool state) { KeyStates.SATTACK_KEY = state; };
	UFUNCTION(BlueprintCallable, Category = SuperFighter)
		void SetDefenceKey(bool state) { KeyStates.DEFENCE_KEY = state; };
	UFUNCTION(BlueprintCallable, Category = SuperFighter)
		void SetJumpKey(bool state) { KeyStates.JUMP_KEY = state; };
};
