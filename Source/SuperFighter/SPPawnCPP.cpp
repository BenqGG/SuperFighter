// Fill out your copyright notice in the Description page of Project Settings.

#include "SuperFighter.h"
#include "SPPawnCPP.h"

// Sets default values
ASPPawnCPP::ASPPawnCPP()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Forces.X = 0.0f;
	Forces.Y = 0.0f;
	Client_Forces.X = 0.0f;
	Client_Forces.Y = 0.0f;
	ClientPosition = GetActorLocation();

	collision_box = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision"));
	RootComponent = collision_box;
	hit_box = CreateDefaultSubobject<UBoxComponent>(TEXT("Hit_Box"));
	hit_box->SetupAttachment(RootComponent);
	hit_box->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	animation = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("Animation"));
	animation->SetupAttachment(RootComponent);
	animation->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));

	StaticAttributes.MovementScale = 10.0f;
	StaticAttributes.AirMovementScale = 1.0f;
	StaticAttributes.WallJumpXModifier = 2.0f;
	StaticAttributes.WallJumpYModifier = 1.5f;

	WorkData.AirJumped = 0;
	WorkData.IsLocal = false;
	WorkData.FacingRight = true;
	WorkData.Injuries = 0;
	WorkData.StrongAttackMeter = 0;
	WorkData.PossitionError = FVector(0.0f, 0.0f, 0.0f);
	WorkData.HitStun = 0.0f;
	WorkData.CurrentDefence = 0.0f;
	WorkData.WasHit = false;
	WorkData.HitForce = FVector(0.0f, 0.0, 0.0f);
	WorkData.AddingForce = false;
	WorkData.AddForce = FVector(0.0f, 0.0, 0.0f);
	WorkData.DefenceDelta = 0.0f;

	ClientHitStun = 0.0f;
	ClientStrongAttackMeter = 0;
	ClientAirJumped = 0;
	ClientCurrentDefence = 0.0f;
	ClientInjuries = 0.0f;
	ClientAnimation = 0;

	Actions.delay = 0.0f;

	
	States.MOVE_RIGHT = false;
	States.MOVE_LEFT = false;
	States.JUMP = false;
	States.JUMP_LEFT_WALL = false;
	States.JUMP_RIGHT_WALL = false;
	States.ON_GROUND = false;
	States.BUSY = false;
	States.CAN_MOVE = true;
	States.CAN_JUMP = true;
	States.CAN_LIGHT_ATTACK = true;
	States.CAN_STRONG_ATTACK = true;
	States.CAN_DEFENCE = true;
	States.CAN_DASH = true;
	States.DEFENCE = false;
	States.STRONG_ATTACK = false;
	States.LIGHT_ATTACK = false;
	States.DASH = false;
	States.SIDE_DASH = false;
	States.UP_DASH = false;
	States.DOWN_DASH = false;
	States.SPOT_DODGE = false;

	ClientStates = States;

	KeyStates.LEFT_KEY = false;
	KeyStates.RIGHT_KEY = false;
	KeyStates.UP_KEY = false;
	KeyStates.DOWN_KEY = false;

	KeyStates.LATTACK_KEY = false;
	KeyStates.SATTACK_KEY = false;
	KeyStates.JUMP_KEY = false;
	KeyStates.DEFENCE_KEY = false;

	LastKeyStates.LEFT_KEY = false;
	LastKeyStates.RIGHT_KEY = false;
	LastKeyStates.UP_KEY = false;
	LastKeyStates.DOWN_KEY = false;

	LastKeyStates.LATTACK_KEY = false;
	LastKeyStates.SATTACK_KEY = false;
	LastKeyStates.JUMP_KEY = false;
	LastKeyStates.DEFENCE_KEY = false;
}

// Called when the game starts or when spawned
void ASPPawnCPP::BeginPlay()
{
	Super::BeginPlay();
	SetUpIdle();
}

// Called every frame
void ASPPawnCPP::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (HasAuthority()) {
		ClientPosition = GetActorLocation();
		//Client_Forces = Forces;
		//ClientStates = States;
		ClientAirJumped = WorkData.AirJumped;
		ClientInjuries = WorkData.Injuries;
		ClientCurrentDefence = WorkData.CurrentDefence;

		ApplyForces(DeltaTime);
		Friction(DeltaTime);
		Gravity(DeltaTime);
		CalculateMovement(DeltaTime);
		ManageDefence(DeltaTime);
		if (IsStun()) {
			ManageStunState(DeltaTime);
		}
		if (States.DEFENCE) {
			DrawDefence();
		}

		GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Yellow, FString::SanitizeFloat(GetController()->PlayerState->ExactPing));
	}
	else {
		ApplyForces(DeltaTime);
		Friction(DeltaTime);
		Gravity(DeltaTime);
		CalculateMovement(DeltaTime);
		ManageDefence(DeltaTime);
		if (IsStun()) {
			ManageStunState(DeltaTime);
		}
		if (States.DEFENCE) {
			DrawDefence();
		}
		CheckKeyStates();
		if (Forces.X == 0.0f && Forces.Y == 0.0f) {
			if (!GetActorLocation().Equals(ClientPosition, 0.0f)) {
				SetActorLocation(ClientPosition, false);
			}	
		}

		//FixPossitionError();
		//GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Yellow, WorkData.PossitionError.ToCompactString());
		/*AController *check = GetController();
		if (IsValid(check)) {
			APlayerState *check2 = check->PlayerState;
			if(IsValid(check2))
			GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Yellow, FString::SanitizeFloat(check->PlayerState->Ping));
		}*/
	}
}

// Called to bind functionality to input
void ASPPawnCPP::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);
}

void ASPPawnCPP::DrawStunMeter_Implementation(float Radius)
{
}


void ASPPawnCPP::PlayClientAnimation_Implementation(int index)
{
}

void ASPPawnCPP::StopDashForces()
{
		if (Forces.X > 0) {
			if ( Forces.X <= Attributes.Dash * 2.0f) {
				Forces.X = 0.0f;
			}
		}
		else {
			if (Forces.X >= -Attributes.Dash* 2.0f) {
				Forces.X = 0.0f;
			}
		}

		if (Forces.Y > 0) {
			if (Forces.Y <= Attributes.Dash* 2.0f) {
				Forces.Y = 0.0f;
			}
		}
		else {
			if (Forces.Y >= -Attributes.Dash* 2.0f) {
				Forces.Y = 0.0f;
			}
		}
}

void ASPPawnCPP::CallEndViewTarget()
{
	if (HasAuthority()) {
		if (IsLocal()) {
			APlayerController* LocalPlayer = UGameplayStatics::GetPlayerController(this, 0);
			LocalPlayer->SetViewTarget(this);		
		}
	}
}

void ASPPawnCPP::Local_Move(float AxisX)
{
	if (CanMove()) {
		if (AxisX == 0 && (States.MOVE_LEFT || States.MOVE_RIGHT)) {
			Server_Move(AxisX);
		}
		else if (AxisX > 0.0f && AxisX > 0.2f && !States.MOVE_RIGHT) {
			Server_Move(AxisX);
		}
		else if (AxisX < 0.0f && AxisX < -0.2f && !States.MOVE_LEFT) {
			Server_Move(AxisX);
		}
	}
}

void ASPPawnCPP::RepNot_UpdatePosition()
{
	if (!HasAuthority() || !States.DASH) {
		SetActorLocation(ClientPosition);
	}
}

void ASPPawnCPP::RepNot_UpdateHitStun()
{
	if (!HasAuthority()) {
		APlayerState*check = UGameplayStatics::GetPlayerController(GetWorld(), 0)->PlayerState;
		if (IsValid(check) && ClientHitStun > 0.0f) {
			WorkData.HitStun = ClientHitStun - (check->Ping / 1000.0f)* 4.0f;
		}
	}
}

void ASPPawnCPP::RepNot_UpdateClientForces()
{
	if (!HasAuthority()) {
		Forces = Client_Forces;
		//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Purple, "Forces");
	}
}

void ASPPawnCPP::RepNot_UpdateStates()
{
	if (/*!HasAuthority() && !States.DASH*/false) {
		bool ReDoDash = false;
		if (ClientStates.MOVE_LEFT && !States.MOVE_LEFT) {
			Move(false);
		}
		else if (ClientStates.MOVE_RIGHT && !States.MOVE_RIGHT) {
			Move(true);
		}
		else if ( (!ClientStates.MOVE_RIGHT && States.MOVE_RIGHT) || (!ClientStates.MOVE_LEFT && States.MOVE_LEFT) ) {
			StopMove();
		}

		if ( (ClientStates.JUMP && !States.JUMP) || (ClientStates.JUMP_LEFT_WALL && !States.JUMP_LEFT_WALL)
			|| (ClientStates.JUMP_RIGHT_WALL && !States.JUMP_RIGHT_WALL) ) {
			Jump();
		}
		//Do we need to check for this?
		/*else if ((!ClientStates.JUMP && States.JUMP) || (!ClientStates.JUMP_LEFT_WALL && States.JUMP_LEFT_WALL)
			|| (!ClientStates.JUMP_RIGHT_WALL && States.JUMP_RIGHT_WALL)) {
			StopJump();
		}*/

		if (ClientStates.DEFENCE && !States.DEFENCE) {
			SetUpDefence();
		}

		if (!ClientStates.DEFENCE && States.DEFENCE) {
			ReleaseDefence();
		}

		States = ClientStates;

		if (ReDoDash) {
			States.DASH = false;
		}
	}
}

void ASPPawnCPP::RepNot_UpdateStrongAttackMeter()
{
	if (!HasAuthority()) {
		WorkData.StrongAttackMeter = ClientStrongAttackMeter;
	}
}

void ASPPawnCPP::RepNot_UpdateAirJumped()
{
	if (!HasAuthority()) {
		WorkData.AirJumped = ClientAirJumped;
	}
}

void ASPPawnCPP::RepNot_UpdateCurrentDefence()
{
	if (!HasAuthority()) {
		WorkData.CurrentDefence = ClientCurrentDefence;
	}
}

void ASPPawnCPP::RepNot_UpdateInjuries()
{
	if (!HasAuthority()) {
		WorkData.Injuries = ClientInjuries;
	}
}

void ASPPawnCPP::RepNot_UpdateAnimation()
{
	if (!HasAuthority()) {
		PlayClientAnimation(ClientAnimation);
	}
}

void ASPPawnCPP::Server_MoveRight_Implementation()
{
	Move(true);
}

bool ASPPawnCPP::Server_MoveRight_Validate()
{
	return true;
}

void ASPPawnCPP::Server_MoveLeft_Implementation()
{
	Move(false);
}

bool ASPPawnCPP::Server_MoveLeft_Validate()
{
	return true;
}

void ASPPawnCPP::Server_StopMove_Implementation()
{
	StopMove();
}

bool ASPPawnCPP::Server_StopMove_Validate()
{
	return true;
}

void ASPPawnCPP::HitPunch(bool FromClient, FVector2D ClientAxisPosition)
{
	if (HasAuthority()) {

		/*FSPHitBoxDetails HBDetails;
		FVector self_position;
		FVector self_bounds;
		FVector temp;


		HBDetails.ExistTime = 0.1f;
		HBDetails.FriendlyFire = false;
		HBDetails.HitStun = 0.0f;
		HBDetails.MultiHit = false;
		HBDetails.Owner = this;

		if (FromClient) {
			HitPosition(ClientAxisPosition, HBDetails.Position, HBDetails.Force);
		}
		else {
			HitPosition(AxisPosition(), HBDetails.Position, HBDetails.Force);
		}
		
		SpawnHitBox(HBDetails);
		
	}
	else {
		Server_HitPunch(AxisPosition());
	*/}
}

void ASPPawnCPP::ManageStunState(float DeltaTime)
{
	
		WorkData.HitStun -= DeltaTime;
		if (WorkData.HitStun <= 0.0f) {
			WorkData.HitStun = 0.0f;
			
			EndStun();
			
		}
		else {
			float StunMeterRadius = (WorkData.HitStun * 50.0f) - 5;
			DrawStunMeter(StunMeterRadius);
		}
}

void ASPPawnCPP::ManageDefence(float DeltaTime)
{
	if (States.DEFENCE) {
		if (WorkData.CurrentDefence > 0.0f) {
			WorkData.DefenceDelta += DeltaTime;
			if (WorkData.DefenceDelta >= 0.2f) {
				WorkData.CurrentDefence -= (Attributes.Defence / 10.0f) * 0.2f;
					WorkData.DefenceDelta -= 0.2f;
					if (WorkData.CurrentDefence <= 0.0f) {
						ReleaseDefence();
						WorkData.CurrentDefence = 0.0f;
					}
			}
		}
	}
	else {
		if (WorkData.CurrentDefence < Attributes.Defence) {
			WorkData.DefenceDelta += DeltaTime;
			if (WorkData.DefenceDelta >= 0.2f) {
				WorkData.CurrentDefence += (Attributes.Defence / 5.0f) * 0.2f;
				WorkData.DefenceDelta -= 0.2f;
				if (WorkData.CurrentDefence > Attributes.Defence) {
					WorkData.CurrentDefence = Attributes.Defence;
				}
			}
		}

	}
}

void ASPPawnCPP::DodgeBlink_Implementation(bool start)
{
}

void ASPPawnCPP::DrawDefence_Implementation()
{
}

void ASPPawnCPP::HitPosition(FVector2D AxisPosition, FVector& Position, FVector& Force)
{
	FVector2D axis_position = AxisPosition;
	FVector self_position;
	self_position = GetActorLocation();

	Position.Y = 35.0f; //Spehere radius
	Force.Y = 0.0f;
	if (axis_position.X > 1 || axis_position.X < -1) axis_position.X = 0.0f;
	if (axis_position.Y > 1 || axis_position.X < -1) axis_position.Y = 0.0f;

	if (axis_position.X == 0 && axis_position.Y == 0) {

		Position.X = self_position.X;
		Position.Z = self_position.Z;
		Force.Z = 0.0f;
		if (WorkData.FacingRight) {
			Position.X += 35.0f;
			Force.X = 1000.0f;
		}
		else {
			Position.X -= 35.0f;
			Force.X = -1000.0f;
		}

		
	}
	else{
		FVector2D temp_axis = axis_position;
		if (temp_axis.X < 0) temp_axis.X *= -1.0f;
		if (temp_axis.Y < 0) temp_axis.Y *= -1.0f;
		Force.X = 1000.0f * axis_position.X;
		Force.Z = 1000.0f * axis_position.Y;

		if (temp_axis.X > temp_axis.Y) {

			Position.X = self_position.X;
			Position.Z = self_position.Z;
			axis_position.X > 0 ? Position.X += 35.0f : Position.X -= 35.0f;
		}
		else {
			Position.X = self_position.X;
			Position.Z = self_position.Z;
			axis_position.Y > 0 ? Position.Z += 35.0f : Position.Z -= 35.0f;
		}
	}
}

void ASPPawnCPP::ChangeAnimationRotation_Implementation()
{
	FRotator rotation(0.0f, 0.0f, 0.0f);
	if(WorkData.FacingRight)
	{
		animation->SetRelativeRotation(rotation, false);
	}
	else {
		rotation = FRotator(0.0f, 180.0f,0.0f);
		animation->SetRelativeRotation(rotation, false);
	}
}

void ASPPawnCPP::SetUpIdle_Implementation()
{

}

void ASPPawnCPP::SpawnHitBox_Implementation(FSPHitBoxDetails l_details)
{

}

FVector2D ASPPawnCPP::AxisPosition_Implementation()
{
	return FVector2D();
}

void ASPPawnCPP::Server_HitPunch_Implementation(FVector2D AxisPosition)
{
	HitPunch(true, AxisPosition);
}

bool ASPPawnCPP::Server_HitPunch_Validate(FVector2D AxisPosition)
{
	return true;
}



void ASPPawnCPP::Server_StopJump_Implementation()
{
	StopJump();
}

bool ASPPawnCPP::Server_StopJump_Validate()
{
	return true;
}

void ASPPawnCPP::Server_Jump_Implementation()
{	
	Jump();
}

bool ASPPawnCPP::Server_Jump_Validate()
{
	return true;
}

void ASPPawnCPP::Server_Move_Implementation(float AxisX)
{	
	if (CanMove())
	{
		if (AxisX >= -1 && AxisX <= 1) {
			if (AxisX == 0 && (States.MOVE_LEFT || States.MOVE_RIGHT)) {
				StopMove();
			}
			else if (AxisX > 0.2f && !States.MOVE_RIGHT) {
				Move(true);
			}
			else if (AxisX < -0.2f && !States.MOVE_LEFT) {
				Move(false);
			}
		}
	}
}

bool ASPPawnCPP::Server_Move_Validate(float AxisX)
{
	return true;
}

void ASPPawnCPP::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	//Here we list the variables we want to replicate + a condition if wanted
	DOREPLIFETIME(ASPPawnCPP, ClientPosition);
	DOREPLIFETIME(ASPPawnCPP, ClientStates);
	DOREPLIFETIME(ASPPawnCPP, Attributes);
	DOREPLIFETIME(ASPPawnCPP, Client_Forces);
	DOREPLIFETIME(ASPPawnCPP, StaticAttributes);
	DOREPLIFETIME(ASPPawnCPP, ClientHitStun);
	DOREPLIFETIME(ASPPawnCPP, ClientStrongAttackMeter);
	DOREPLIFETIME(ASPPawnCPP, ClientAirJumped);
	DOREPLIFETIME(ASPPawnCPP, ClientCurrentDefence);
	DOREPLIFETIME(ASPPawnCPP, ClientInjuries);
	DOREPLIFETIME(ASPPawnCPP, ClientAnimation);
}


void ASPPawnCPP::Jump()
{	
	if (HasAuthority()) {
			if (CanJump()) {
				if (GroundUnderFeet()) {
					if(!States.JUMP) States.JUMP = true;
					GetWorldTimerManager().ClearTimer(WorkData.JumpTimer);
					GetWorldTimerManager().SetTimer(WorkData.JumpTimer, this, &ASPPawnCPP::StopJump, Attributes.JumpTime, false);
					Actions.Jump.ExecuteIfBound();
					Client_Jump(GetSendPosition(), 0);
				}
				else if (GroundNextToFeet(true)) {
					if (!States.JUMP_RIGHT_WALL) States.JUMP_RIGHT_WALL = true;
					GetWorldTimerManager().ClearTimer(WorkData.JumpTimer);
					GetWorldTimerManager().SetTimer(WorkData.JumpTimer, this, &ASPPawnCPP::StopJump, Attributes.JumpTime, false);
					Actions.Jump.ExecuteIfBound();
					Client_Jump(GetSendPosition(), 1);
				}
				else if (GroundNextToFeet(false)) {
					if(!States.JUMP_LEFT_WALL) States.JUMP_LEFT_WALL = true;
					GetWorldTimerManager().ClearTimer(WorkData.JumpTimer);
					GetWorldTimerManager().SetTimer(WorkData.JumpTimer, this, &ASPPawnCPP::StopJump, Attributes.JumpTime, false);
					Actions.Jump.ExecuteIfBound();
					Client_Jump(GetSendPosition(), 2);
				}
				else if (WorkData.AirJumped < Attributes.AirJumpAmount) {
					WorkData.AirJumped++;
					if (States.MOVE_LEFT) {
						if(!States.JUMP_RIGHT_WALL) States.JUMP_RIGHT_WALL = true;
						GetWorldTimerManager().ClearTimer(WorkData.JumpTimer);
						GetWorldTimerManager().SetTimer(WorkData.JumpTimer, this, &ASPPawnCPP::StopJump, Attributes.JumpTime, false);
						Actions.Jump.ExecuteIfBound();
						Client_Jump(GetSendPosition(), 1);
					}
					else if (States.MOVE_RIGHT) {
						if(!States.JUMP_LEFT_WALL) States.JUMP_LEFT_WALL = true;
						GetWorldTimerManager().ClearTimer(WorkData.JumpTimer);
						GetWorldTimerManager().SetTimer(WorkData.JumpTimer, this, &ASPPawnCPP::StopJump, Attributes.JumpTime, false);
						Actions.Jump.ExecuteIfBound();
						Client_Jump(GetSendPosition(), 2);
					}
					else {
						if(!States.JUMP) States.JUMP = true;
						GetWorldTimerManager().ClearTimer(WorkData.JumpTimer);
						GetWorldTimerManager().SetTimer(WorkData.JumpTimer, this, &ASPPawnCPP::StopJump, Attributes.JumpTime, false);
						Actions.Jump.ExecuteIfBound();
						Client_Jump(GetSendPosition(), 0);
					}
			}
		}
	}
	else {
		GetWorldTimerManager().ClearTimer(WorkData.JumpTimer);
		GetWorldTimerManager().SetTimer(WorkData.JumpTimer, this, &ASPPawnCPP::StopJump, Attributes.JumpTime, false);
		Actions.Jump.ExecuteIfBound();
	}
}

void ASPPawnCPP::MakeBusy()
{
	if (HasAuthority() && States.BUSY != true) States.BUSY = true;
}

void ASPPawnCPP::UnBusy()
{
	if (HasAuthority() && States.BUSY != false) States.BUSY = false;
}

void ASPPawnCPP::StopJump()
{
	if (HasAuthority()) {
		if (CanStopJump()) {
			if (States.JUMP) {
				States.JUMP = false;
				Actions.StopJump.ExecuteIfBound();
			}
			if (States.JUMP_LEFT_WALL) {
				States.JUMP_LEFT_WALL = false;
				Actions.StopJump.ExecuteIfBound();
			}
			if (States.JUMP_RIGHT_WALL) {
				States.JUMP_RIGHT_WALL = false;
				Actions.StopJump.ExecuteIfBound();
			}
			GetWorldTimerManager().ClearTimer(WorkData.JumpTimer);
		}
	}
	else {
		GetWorldTimerManager().ClearTimer(WorkData.ClientTimer);
		if (States.JUMP || States.JUMP_LEFT_WALL || States.JUMP_RIGHT_WALL) {
			States.JUMP = false;
			States.JUMP_LEFT_WALL = false;
			States.JUMP_RIGHT_WALL = false;
			GetWorldTimerManager().ClearTimer(WorkData.JumpTimer);
			Actions.StopJump.ExecuteIfBound();
		}
	}
}

void ASPPawnCPP::LightAttack(int index)
{
	if (HasAuthority()) {
		if (CanLightAttack()) {
			if (index == 0) {
				FVector2D CurrentAxis = AxisPosition();
				FVector2D AbsCurrentAxis;
				if (CurrentAxis.X < 0) AbsCurrentAxis.X = CurrentAxis.X * -1; else AbsCurrentAxis.X = CurrentAxis.X;
				if (CurrentAxis.Y < 0) AbsCurrentAxis.Y = CurrentAxis.Y * -1; else AbsCurrentAxis.Y = CurrentAxis.Y;

				if ((CurrentAxis.X == 0.0f && CurrentAxis.Y == 0.0f) || (AbsCurrentAxis.X > AbsCurrentAxis.Y)) {
					if (States.ON_GROUND)
						Actions.LightAttack.ExecuteIfBound();
					else
						Actions.AirLightAttack.ExecuteIfBound();
				}
				else {
					if (CurrentAxis.Y > 0.0f) {
						if (States.ON_GROUND)
							Actions.UpperLightAttack.ExecuteIfBound();
						else
							Actions.AirUpperLightAttack.ExecuteIfBound();
					}
					else {
						if (States.ON_GROUND)
							Actions.DownLightAttack.ExecuteIfBound();
						else 
							Actions.AirDownLightAttack.ExecuteIfBound();
					}
				}
			}
			else if(index == 1) {
				if (States.ON_GROUND)
					Actions.LightAttack.ExecuteIfBound();
				else
					Actions.AirLightAttack.ExecuteIfBound();
			}
			else if (index == 2) {
				if (States.ON_GROUND)
					Actions.UpperLightAttack.ExecuteIfBound();
				else
					Actions.AirUpperLightAttack.ExecuteIfBound();
			}
			else if (index == 3) {
				if (States.ON_GROUND)
					Actions.DownLightAttack.ExecuteIfBound();
				else
					Actions.AirDownLightAttack.ExecuteIfBound();
			}
		}
	}
	else {
		if (CanLightAttack()) {
			FVector2D CurrentAxis = AxisPosition();
			FVector2D AbsCurrentAxis;
			if (CurrentAxis.X < 0) AbsCurrentAxis.X = CurrentAxis.X * -1; else AbsCurrentAxis.X = CurrentAxis.X;
			if (CurrentAxis.Y < 0) AbsCurrentAxis.Y = CurrentAxis.Y * -1; else AbsCurrentAxis.Y = CurrentAxis.Y;

			if ((CurrentAxis.X == 0.0f && CurrentAxis.Y == 0.0f) || (AbsCurrentAxis.X > AbsCurrentAxis.Y)) {
				Server_LightAttack();
			}
			else {
				if (CurrentAxis.Y > 0.0f) {
					Server_UpperLightAttack();
				}
				else {
					Server_DownLightAttack();
				}
			}
		}
	}
}

void ASPPawnCPP::StrongAttack(int index)
{
	if (HasAuthority()) {
		if (CanStrongAttack()) {

			SetUpStrongAttack();

			if (index == 0) {
				FVector2D CurrentAxis = AxisPosition();
				FVector2D AbsCurrentAxis;
				if (CurrentAxis.X < 0) AbsCurrentAxis.X = CurrentAxis.X * -1; else AbsCurrentAxis.X = CurrentAxis.X;
				if (CurrentAxis.Y < 0) AbsCurrentAxis.Y = CurrentAxis.Y * -1; else AbsCurrentAxis.Y = CurrentAxis.Y;

				if (CurrentAxis.X == 0.0f && CurrentAxis.Y == 0.0f) {
					if (States.ON_GROUND)
						Actions.StrongAttack.ExecuteIfBound();
					else
						Actions.AirStrongAttack.ExecuteIfBound();
				}
				else if (AbsCurrentAxis.X > AbsCurrentAxis.Y) {
					if (States.ON_GROUND)
						Actions.SideStrongAttack.ExecuteIfBound();
					else
						Actions.AirSideStrongAttack.ExecuteIfBound();
				}
				else {
					if (CurrentAxis.Y > 0.0f) {
						if (States.ON_GROUND)
							Actions.UpperStrongAttack.ExecuteIfBound();
						else
							Actions.AirUpperStrongAttack.ExecuteIfBound();
					}
					else {
						if (States.ON_GROUND)
							Actions.DownStrongAttack.ExecuteIfBound();
						else
							Actions.AirDownStrongAttack.ExecuteIfBound();
					}
				}
			}
			else if (index == 1) {
				if (States.ON_GROUND)
					Actions.StrongAttack.ExecuteIfBound();
				else
					Actions.AirStrongAttack.ExecuteIfBound();
			}
			else if (index == 2) {
				if (States.ON_GROUND)
					Actions.SideStrongAttack.ExecuteIfBound();
				else
					Actions.AirSideStrongAttack.ExecuteIfBound();
			}
			else if (index == 3) {
				if (States.ON_GROUND)
					Actions.UpperStrongAttack.ExecuteIfBound();
				else
					Actions.AirUpperStrongAttack.ExecuteIfBound();
			}
			else if (index == 4) {
				if (States.ON_GROUND)
					Actions.DownStrongAttack.ExecuteIfBound();
				else
					Actions.AirDownStrongAttack.ExecuteIfBound();
			}
		}
	}
	else {
		if (CanStrongAttack()) {
			FVector2D CurrentAxis = AxisPosition();
			FVector2D AbsCurrentAxis;
			if (CurrentAxis.X < 0) AbsCurrentAxis.X = CurrentAxis.X * -1; else AbsCurrentAxis.X = CurrentAxis.X;
			if (CurrentAxis.Y < 0) AbsCurrentAxis.Y = CurrentAxis.Y * -1; else AbsCurrentAxis.Y = CurrentAxis.Y;

			if (CurrentAxis.X == 0.0f && CurrentAxis.Y == 0.0f) {
					Server_StrongAttack();
			}
			else if (AbsCurrentAxis.X > AbsCurrentAxis.Y) {
				Server_SideStrongAttack();
			}
			else if(CurrentAxis.Y > 0.0f){
				Server_UpperStrongAttack();
			}
			else {
				Server_DownStrongAttack();
			}
		}
	}	
}


void ASPPawnCPP::Server_StrongAttack_Implementation()
{
	StrongAttack(1);
}

bool ASPPawnCPP::Server_StrongAttack_Validate()
{
	return true;
}

void ASPPawnCPP::Server_SideStrongAttack_Implementation()
{
	StrongAttack(2);
}

bool ASPPawnCPP::Server_SideStrongAttack_Validate()
{
	return true;
}

void ASPPawnCPP::Server_UpperStrongAttack_Implementation()
{
	StrongAttack(3);
}

bool ASPPawnCPP::Server_UpperStrongAttack_Validate()
{
	return true;
}

void ASPPawnCPP::Server_DownStrongAttack_Implementation()
{
	StrongAttack(4);
}

bool ASPPawnCPP::Server_DownStrongAttack_Validate()
{
	return true;
}

void ASPPawnCPP::ReleaseStrongAttack()
{
	if (HasAuthority()) {
		if (CanReleaseStrongAttack()) {
			ClearStrongAttack();
			Actions.RealeaseStrongAttack.ExecuteIfBound();
		}
	}
	else {
		if (CanReleaseStrongAttack()) {
			Server_ReleaseStrongAttack();
		}
	}
}

void ASPPawnCPP::Server_ReleaseStrongAttack_Implementation()
{
	ReleaseStrongAttack();
}

bool ASPPawnCPP::Server_ReleaseStrongAttack_Validate()
{
	return true;
}

void ASPPawnCPP::Server_LightAttack_Implementation()
{
	LightAttack(1);
}

bool ASPPawnCPP::Server_LightAttack_Validate()
{
	return true;
}

void ASPPawnCPP::Server_UpperLightAttack_Implementation()
{
	LightAttack(2);
}

bool ASPPawnCPP::Server_UpperLightAttack_Validate()
{
	return true;
}

void ASPPawnCPP::Server_DownLightAttack_Implementation()
{
	LightAttack(3);
}

bool ASPPawnCPP::Server_DownLightAttack_Validate()
{
	return true;
}

void ASPPawnCPP::Defence(int index)
{
	if (HasAuthority()) {
		FVector CurrentPosition = GetActorLocation();
		FVector2D SendPosition;
		SendPosition.X = CurrentPosition.X;
		SendPosition.Y = CurrentPosition.Z;

		if (index == 0) {
			FVector2D CurrentAxis = AxisPosition();
			FVector2D AbsCurrentAxis;
			if (CurrentAxis.X < 0) AbsCurrentAxis.X = CurrentAxis.X * -1; else AbsCurrentAxis.X = CurrentAxis.X;
			if (CurrentAxis.Y < 0) AbsCurrentAxis.Y = CurrentAxis.Y * -1; else AbsCurrentAxis.Y = CurrentAxis.Y;

			if ((CurrentAxis.X == 0.0f && CurrentAxis.Y == 0.0f) && !States.MOVE_RIGHT && !States.MOVE_LEFT) {
				if (CanDefence()) 
				{	
					SetUpDefence();
					Actions.Defence.ExecuteIfBound();
				}
			}
			else if (AbsCurrentAxis.X >= AbsCurrentAxis.Y) {
				if (CanDash()) {
					
					States.SIDE_DASH = true;
					SetUpDash();
				}
			}
			else {
				if (CurrentAxis.Y > 0.0f) {
					if (CanDash()) {
					
						States.UP_DASH = true;
						SetUpDash();
					}
				}
				else {
					if (CanDash()) {
						if (States.ON_GROUND) {
							
							States.SPOT_DODGE = true;
							SetUpDash();
						}
							
						else {
							
							States.DOWN_DASH = true;
							SetUpDash();
						}
					}
				}
			}
		}
		else if (index == 1) {
			if (CanDefence()) {
				SetUpDefence();
				Actions.Defence.ExecuteIfBound();
			}
		}
		else if (index == 2 || index == 3) {
			if (CanDash()) {
				if (index == 2) {
					States.SIDE_DASH = true;
					Client_Dash(SendPosition, 0);
				}
				else {
					States.UP_DASH = true;
					Client_Dash(SendPosition, 1);
				}

				SetUpDash();
			}
		}
		else if (index == 4) {
			if (CanDash()) {
				if (States.ON_GROUND) {
					States.SPOT_DODGE = true;
					Client_Dash(SendPosition, 3);
					SetUpDash();
				}
				else {
					States.DOWN_DASH = true;
					Client_Dash(SendPosition, 2);
					SetUpDash();
				}
					
			}
		}
	}
	else {
		FVector2D CurrentAxis = AxisPosition();
		FVector2D AbsCurrentAxis;
		if (CurrentAxis.X < 0) AbsCurrentAxis.X = CurrentAxis.X * -1; else AbsCurrentAxis.X = CurrentAxis.X;
		if (CurrentAxis.Y < 0) AbsCurrentAxis.Y = CurrentAxis.Y * -1; else AbsCurrentAxis.Y = CurrentAxis.Y;

		if ((CurrentAxis.X == 0.0f && CurrentAxis.Y == 0.0f) && !States.MOVE_RIGHT && !States.MOVE_LEFT) {
			if (CanDefence()) {
				Server_Defence();
				
			}
		}
		else if (AbsCurrentAxis.X >= AbsCurrentAxis.Y && CanDash()) {
			Server_SideDash();
		}
		else if(CanDash()){
			if (CurrentAxis.Y > 0.0f) {
				Server_UpDash();
			}
			else {
				Server_DownDash();
			}
		}
	}
}

void ASPPawnCPP::SetClientAnimation(int index)
{
	if (HasAuthority()) {
		ClientAnimation = index;
	}
}

void ASPPawnCPP::Server_Defence_Implementation()
{
	Defence(1);
}

bool ASPPawnCPP::Server_Defence_Validate()
{
	return true;
}

void ASPPawnCPP::Server_SideDash_Implementation()
{
	Defence(2);
}

bool ASPPawnCPP::Server_SideDash_Validate()
{
	return true;
}

void ASPPawnCPP::Server_UpDash_Implementation()
{
	Defence(3);
}

bool ASPPawnCPP::Server_UpDash_Validate()
{
	return true;
}

void ASPPawnCPP::Server_DownDash_Implementation()
{
	Defence(4);
}

bool ASPPawnCPP::Server_DownDash_Validate()
{
	return true;
}

void ASPPawnCPP::ReleaseDefence()
{
	if (HasAuthority()) {
		if (CanReleaseDefence())
		{
			ClearDefence();
			Actions.ReleaseDefence.ExecuteIfBound();
			
			/*Replaced by ManageDefence
			if (WorkData.CurrentDefence < Attributes.Defence) {
				GetWorldTimerManager().SetTimer(WorkData.DefenceTimer, this, &ASPPawnCPP::ReplenishDefence, 1.0f, true);
			}*/
		}
	}
	else {
		ClearDefence();
		Actions.ReleaseDefence.ExecuteIfBound();
	}
}

void ASPPawnCPP::Server_ReleaseDefence_Implementation()
{
	ReleaseDefence();
}

bool ASPPawnCPP::Server_ReleaseDefence_Validate()
{
	return true;
}

void ASPPawnCPP::Hit(float forceX, float forceY)
{
	if (HasAuthority()) {
		Forces.X += forceX;
		Forces.Y += forceY;
	}
}

bool ASPPawnCPP::GroundNextToFeet_Implementation(bool right) const
{
	//We dont use c++ version for now only BP for ray casy
	return false;
}

bool ASPPawnCPP::GroundUnderFeet_Implementation() const
{
	//We dont realy use the c++ version
	return false;
}

void ASPPawnCPP::ChangeAnimation(FSPAnimationDetails details)
{
	collision_box->SetBoxExtent(FVector(details.CollisionBox.X, 10, details.CollisionBox.Y), true);
	hit_box->SetBoxExtent(FVector(details.HitBox.X, 10, details.HitBox.Y), true);
	animation->SetFlipbook(details.Flipbook);
	animation->PlayFromStart();
}

void ASPPawnCPP::ChangeMovementSpeed(float speed)
{
	Attributes.MoveSpeed = speed;
}

void ASPPawnCPP::AddForce(FVector force)
{
	if (HasAuthority()) {
		WorkData.AddingForce = true;
		WorkData.AddForce = force;
	}
}

void ASPPawnCPP::Friction(float DeltaTime)
{
		if(!States.DASH && !States.MOVE_LEFT && !States.MOVE_RIGHT)
		{
			if (Forces.X > 0.0f) {
				if (GroundUnderFeet()) {
					Forces.X -= ValuePerSecond(Attributes.Friction, DeltaTime);
				}
				else {
					Forces.X -= ValuePerSecond(Attributes.AirFriction, DeltaTime);
				}
				if (Forces.X < 0.0f) {
					Forces.X = 0.0f;
				}
			}
			else if (Forces.X < 0.0f) {
				if (GroundUnderFeet()) {
					Forces.X += ValuePerSecond(Attributes.Friction, DeltaTime);
				}
				else {
					Forces.X += ValuePerSecond(Attributes.AirFriction, DeltaTime);
				}
				if (Forces.X > 0.0f) {
					Forces.X = 0.0f;
				}
			}
		}
}

void ASPPawnCPP::Gravity(float DeltaTime)
{
	if (!GroundUnderFeet() && !States.DASH && !States.JUMP && !States.JUMP_LEFT_WALL && !States.JUMP_RIGHT_WALL) {
		if ((States.LIGHT_ATTACK || States.STRONG_ATTACK) && Forces.Y < 0.0f)
			Forces.Y -= ValuePerSecond(Attributes.Gravity, DeltaTime) / 10.0f;
		else
			Forces.Y -= ValuePerSecond(Attributes.Gravity, DeltaTime);
		}
}

void ASPPawnCPP::FixPossitionError()
{
	if (!HasAuthority()) {
		GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Yellow, FString::SanitizeFloat(WorkData.PossitionError.X));
		FVector current_position = this->GetActorLocation();
		if (WorkData.PossitionError.X > 0.0f) {
			if (WorkData.PossitionError.X <= 10.0f || WorkData.PossitionError.X > 50.0f) {
				WorkData.PossitionError.X = 0.0f;
				current_position.X += WorkData.PossitionError.X;
			}
			else {
				WorkData.PossitionError.X -= 10.0f;
				current_position.X += 10.0f;
			}
		}
		else if (WorkData.PossitionError.X < 0.0f) {
			if (WorkData.PossitionError.X >= -10.0f || WorkData.PossitionError.X < -50.0f) {
				WorkData.PossitionError.X = 0.0f;
				current_position.X += WorkData.PossitionError.X;
			}
			else {
				WorkData.PossitionError.X += 10.0f;
				current_position.X += -10.0f;
			}
		}

		if (WorkData.PossitionError.Z > 0.0f) {
			if (WorkData.PossitionError.Z <= 10.0f || WorkData.PossitionError.X > 50.0f) {
				WorkData.PossitionError.Z = 0.0f;
				current_position.Z += WorkData.PossitionError.Z;
			}
			else {
				WorkData.PossitionError.Z -= 10.0f;
				current_position.Z += 10.0f;
			}
		}
		else if (WorkData.PossitionError.Z < 0.0f) {
			if (WorkData.PossitionError.Z >= -10.0f || WorkData.PossitionError.X < -50.0f) {
				WorkData.PossitionError.Z = 0.0f;
				current_position.X += WorkData.PossitionError.Z;
			}
			else {
				WorkData.PossitionError.Z += 10.0f;
				current_position.Z += -10.0f;
			}
		}

		this->SetActorLocation(current_position);
	}
}

void ASPPawnCPP::UpgradeStrongAttackMeter()
{
	WorkData.StrongAttackMeter++;
}

void ASPPawnCPP::ClearStatesWhileHit()
{
	if (HasAuthority()) {

		GetWorldTimerManager().ClearTimer(DelayTimer);

		if(States.MOVE_RIGHT)States.MOVE_RIGHT = false;
		if (States.MOVE_LEFT)States.MOVE_LEFT = false;
		if (States.JUMP) {
			States.JUMP = false;
			GetWorldTimerManager().ClearTimer(WorkData.JumpTimer);
		}
		if (States.JUMP_LEFT_WALL) {
			States.JUMP_LEFT_WALL = false;
			GetWorldTimerManager().ClearTimer(WorkData.JumpTimer);
		}
		if (States.JUMP_RIGHT_WALL) {
			States.JUMP_RIGHT_WALL = false;
			GetWorldTimerManager().ClearTimer(WorkData.JumpTimer);
		}
		if (States.DEFENCE) {
			States.DEFENCE = false;
			//Replaced By Manage Defence
			//GetWorldTimerManager().ClearTimer(WorkData.DefenceTimer);
			//GetWorldTimerManager().SetTimer(WorkData.DefenceTimer, this, &ASPPawnCPP::ReplenishDefence, 1.0f, true);
		}
		if (States.STRONG_ATTACK) {
			States.STRONG_ATTACK = false;
			GetWorldTimerManager().ClearTimer(WorkData.DashTimer);
		}
		if (States.LIGHT_ATTACK) {
			States.LIGHT_ATTACK = false;
			GetWorldTimerManager().ClearTimer(WorkData.LightAttackTimer);
		}
	
		if (!States.CAN_MOVE)States.CAN_MOVE = true;
		if (!States.CAN_JUMP)States.CAN_JUMP = true;
		if (!States.CAN_LIGHT_ATTACK)States.CAN_LIGHT_ATTACK = true;
		if (!States.CAN_STRONG_ATTACK)States.CAN_STRONG_ATTACK = true;
		if (!States.CAN_DEFENCE)States.CAN_DEFENCE = true;
		
		
	}
}

void ASPPawnCPP::SetUpDefence()
{
	if (HasAuthority()) {
		if (!States.DEFENCE) States.DEFENCE = true;

		if (States.JUMP) {
			States.JUMP = false;
			GetWorldTimerManager().ClearTimer(WorkData.JumpTimer);
		}
		if (States.JUMP_LEFT_WALL) {
			States.JUMP_LEFT_WALL = false;
			GetWorldTimerManager().ClearTimer(WorkData.JumpTimer);
		}
		if (States.JUMP_RIGHT_WALL) {
			States.JUMP_RIGHT_WALL = false;
			GetWorldTimerManager().ClearTimer(WorkData.JumpTimer);
		}

		if (States.CAN_MOVE) States.CAN_MOVE = false;
		if (States.CAN_JUMP) States.CAN_JUMP = false;
		if (States.CAN_LIGHT_ATTACK) States.CAN_LIGHT_ATTACK = false;
		if (States.CAN_STRONG_ATTACK) States.CAN_STRONG_ATTACK = false;
		if (States.CAN_DEFENCE) States.CAN_DEFENCE = false;
		if (States.CAN_DASH) States.CAN_DASH = false;
		
		//Replaced By ManageDefence
		//GetWorldTimerManager().ClearTimer(WorkData.DefenceTimer);
		//GetWorldTimerManager().SetTimer(WorkData.DefenceTimer, this, &ASPPawnCPP::UseDefence, 1.0f, true);
	}
	else {
		GetWorldTimerManager().ClearTimer(WorkData.JumpTimer);
		Actions.Defence.ExecuteIfBound();
	}
}

void ASPPawnCPP::ClearDefence()
{
	if (HasAuthority()) {
		if (States.DEFENCE) States.DEFENCE = false;

		if (!States.CAN_MOVE)			States.CAN_MOVE = true;
		if (!States.CAN_JUMP)			States.CAN_JUMP = true;
		if (!States.CAN_LIGHT_ATTACK)	States.CAN_LIGHT_ATTACK = true;
		if (!States.CAN_STRONG_ATTACK)	States.CAN_STRONG_ATTACK = true;
		if (!States.CAN_DEFENCE)		States.CAN_DEFENCE = true;
		if (!States.CAN_DASH)			States.CAN_DASH = true;

		//Replaced by manage defence
		//GetWorldTimerManager().ClearTimer(WorkData.DefenceTimer);
	}
	else {
		States.DEFENCE = false;
		ClientStates.DEFENCE = false;
	}
}

void ASPPawnCPP::UseDefence()
{
	//Not in use replaced by ManageDefence
	if (HasAuthority()) {
		WorkData.CurrentDefence -= Attributes.Defence / 20.0f;
		if (WorkData.CurrentDefence <= 0.0f) {
			WorkData.CurrentDefence = 0.0f;
		//	GetWorldTimerManager().ClearTimer(WorkData.DefenceTimer);
			ReleaseDefence();
		}
	}
}

void ASPPawnCPP::ReplenishDefence()
{
	//Not in use replaced by ManagDefence
	if (HasAuthority()) {
		if (WorkData.CurrentDefence < Attributes.Defence) {
			WorkData.CurrentDefence += Attributes.Defence / 10.0f;
			if (WorkData.CurrentDefence >= Attributes.Defence) {
				WorkData.CurrentDefence = Attributes.Defence;
				//GetWorldTimerManager().ClearTimer(WorkData.DefenceTimer);
			}
		}
		//else {
			//GetWorldTimerManager().ClearTimer(WorkData.DefenceTimer);
		//}
	}
}

void ASPPawnCPP::SetUpDash()
{
		
		if (!States.DASH) States.DASH = true;

		if (States.MOVE_RIGHT) States.MOVE_RIGHT = false;
		if (States.MOVE_LEFT)States.MOVE_LEFT = false;
		if (States.JUMP) {
			States.JUMP = false;
			GetWorldTimerManager().ClearTimer(WorkData.JumpTimer);
		}
		if (States.JUMP_LEFT_WALL) {
			States.JUMP_LEFT_WALL = false;
			GetWorldTimerManager().ClearTimer(WorkData.JumpTimer);
		}
		if (States.JUMP_RIGHT_WALL) {
			States.JUMP_RIGHT_WALL = false;
			GetWorldTimerManager().ClearTimer(WorkData.JumpTimer);
		}

		if (HasAuthority()) {
			if (States.CAN_MOVE)				States.CAN_MOVE = false;
			if (States.CAN_JUMP)				States.CAN_JUMP = false;
			if (States.CAN_LIGHT_ATTACK)		States.CAN_LIGHT_ATTACK = false;
			if (States.CAN_STRONG_ATTACK)		States.CAN_STRONG_ATTACK = false;
			if (States.CAN_DEFENCE)				States.CAN_DEFENCE = false;
			if (States.CAN_DASH)				States.CAN_DASH = false;
		}
		
		if (States.SPOT_DODGE) {
			GetWorldTimerManager().SetTimer(WorkData.DashTimer, this, &ASPPawnCPP::StopDash, Attributes.SpotDodgeTime, false);
			Actions.SpotDodge.ExecuteIfBound();
		}
		else {
			GetWorldTimerManager().SetTimer(WorkData.DashTimer, this, &ASPPawnCPP::StopDash, Attributes.DashTime, false);
			if (HasAuthority()) {
				if(States.ON_GROUND)
					Actions.Dash.ExecuteIfBound();
				else
					Actions.AirDash.ExecuteIfBound();
			}
			else {
				if (GroundUnderFeet())
					Actions.Dash.ExecuteIfBound();
				else
					Actions.AirDash.ExecuteIfBound();
			}
		}

		DodgeBlink(true);
}

void ASPPawnCPP::StopDash()
{
	
		if (States.DASH) States.DASH = false;

		if (!States.CAN_MOVE)				States.CAN_MOVE = true;
		if (!States.CAN_JUMP)				States.CAN_JUMP = true;
		if (!States.CAN_LIGHT_ATTACK)		States.CAN_LIGHT_ATTACK = true;
		if (!States.CAN_STRONG_ATTACK)		States.CAN_STRONG_ATTACK = true;
		if (!States.CAN_DEFENCE)			States.CAN_DEFENCE = true;

		if (States.CAN_DASH) States.CAN_DASH = false;
		
		GetWorldTimerManager().ClearTimer(WorkData.DashTimer);
		if(HasAuthority())
		GetWorldTimerManager().SetTimer(WorkData.DashTimer, this, &ASPPawnCPP::DashColdown, 1.0f, false);

		DodgeBlink(false);
		StopDashForces();
		DashEnd();
}

void ASPPawnCPP::DashColdown()
{
	if (HasAuthority()) {
		if (!States.CAN_DASH) States.CAN_DASH = true;
		GetWorldTimerManager().ClearTimer(WorkData.DashTimer);
	}
}

void ASPPawnCPP::StartLightAttack(float time)
{
	if (HasAuthority()) {
		if (!States.LIGHT_ATTACK) States.LIGHT_ATTACK = true;

		if (States.MOVE_RIGHT) States.MOVE_RIGHT = false;
		if (States.MOVE_LEFT)States.MOVE_LEFT = false;
		if (States.JUMP) {
			States.JUMP = false;
			GetWorldTimerManager().ClearTimer(WorkData.JumpTimer);
		}
		if (States.JUMP_LEFT_WALL) {
			States.JUMP_LEFT_WALL = false;
			GetWorldTimerManager().ClearTimer(WorkData.JumpTimer);
		}
		if (States.JUMP_RIGHT_WALL) {
			States.JUMP_RIGHT_WALL = false;
			GetWorldTimerManager().ClearTimer(WorkData.JumpTimer);
		}

		if (States.CAN_MOVE)			States.CAN_MOVE = false;
		if (States.CAN_JUMP)			States.CAN_JUMP = false;
		if (States.CAN_DEFENCE)			States.CAN_DEFENCE = false;
		if (States.CAN_DASH)			States.CAN_DASH = false;
		if (States.CAN_STRONG_ATTACK)	States.CAN_STRONG_ATTACK = false;
		if (States.CAN_LIGHT_ATTACK)	States.CAN_LIGHT_ATTACK = false;

		GetWorldTimerManager().ClearTimer(WorkData.LightAttackTimer);
		GetWorldTimerManager().SetTimer(WorkData.LightAttackTimer, this, &ASPPawnCPP::EndLightAttack, time, false);
	}
}

void ASPPawnCPP::EndLightAttack()
{
	if (HasAuthority()) {
		
		if (States.LIGHT_ATTACK) States.LIGHT_ATTACK = false;

		if (!States.CAN_MOVE)			States.CAN_MOVE = true;
		if (!States.CAN_JUMP)			States.CAN_JUMP = true;
		if (!States.CAN_DEFENCE)		States.CAN_DEFENCE = true;
		if (!States.CAN_DASH)			States.CAN_DASH = true;
		if (!States.CAN_STRONG_ATTACK)	States.CAN_STRONG_ATTACK = true;
		if (!States.CAN_LIGHT_ATTACK)	States.CAN_LIGHT_ATTACK = true;
		
		GetWorldTimerManager().ClearTimer(WorkData.LightAttackTimer);
	}
}

void ASPPawnCPP::SetUpStrongAttack()
{
	if (HasAuthority()) {
		if (!States.STRONG_ATTACK) States.STRONG_ATTACK = true;

		if(States.MOVE_RIGHT) States.MOVE_RIGHT = false;
		if(States.MOVE_LEFT )States.MOVE_LEFT = false;
		if (States.JUMP) {
			States.JUMP = false;
			GetWorldTimerManager().ClearTimer(WorkData.JumpTimer);
		} 
		if (States.JUMP_LEFT_WALL) {
			States.JUMP_LEFT_WALL = false;
			GetWorldTimerManager().ClearTimer(WorkData.JumpTimer);
		}
		if (States.JUMP_RIGHT_WALL) {
			States.JUMP_RIGHT_WALL = false;
			GetWorldTimerManager().ClearTimer(WorkData.JumpTimer);
		}
		if (States.LIGHT_ATTACK) {
			States.LIGHT_ATTACK = false;
			GetWorldTimerManager().ClearTimer(WorkData.LightAttackTimer);
		}
		

		if(States.CAN_MOVE) States.CAN_MOVE = false;
		if(States.CAN_JUMP) States.CAN_JUMP = false;
		if(States.CAN_LIGHT_ATTACK) States.CAN_LIGHT_ATTACK = false;
		if(States.CAN_STRONG_ATTACK) States.CAN_STRONG_ATTACK = false;
		if(States.CAN_DEFENCE) States.CAN_DEFENCE = false;
		if(States.CAN_DASH) States.CAN_DASH  = false;
		
		WorkData.StrongAttackMeter = 0;
		GetWorldTimerManager().ClearTimer(WorkData.StrongAttackTimer);
		GetWorldTimerManager().SetTimer(WorkData.StrongAttackTimer, this, &ASPPawnCPP::UpgradeStrongAttackMeter, 0.05f, true);
	}
}

void ASPPawnCPP::ClearStrongAttack()
{
	if (HasAuthority()) {
		if (States.STRONG_ATTACK) States.STRONG_ATTACK = false;

		if (!States.CAN_MOVE)			States.CAN_MOVE = true;
		if (!States.CAN_JUMP)			States.CAN_JUMP = true;
		if (!States.CAN_LIGHT_ATTACK)	States.CAN_LIGHT_ATTACK = true;
		if (!States.CAN_STRONG_ATTACK)	States.CAN_STRONG_ATTACK = true;
		if (!States.CAN_DEFENCE)		States.CAN_DEFENCE = true;
		if (!States.CAN_DASH)			States.CAN_DASH = true;

		GetWorldTimerManager().ClearTimer(WorkData.StrongAttackTimer);
	}
}



void ASPPawnCPP::ApplyForces(float DeltaTime)
{
	FVector current_location;


		if (Forces.X != 0.0f) {
			current_location = GetActorLocation();
			current_location.X += Forces.X * (DeltaTime / 1.0f);
			if (!SetActorLocation(current_location, true, nullptr)) {
				Forces.X = 0.0f;
			}
		}
		if (Forces.Y != 0.0f) {
			current_location = GetActorLocation();
			current_location.Z += Forces.Y * (DeltaTime / 1.0f);
			if (!SetActorLocation(current_location, true, nullptr)) {
				if (Forces.Y < 0) { 
					WorkData.AirJumped = 0;
				}
				Forces.Y = 0.0f;
			}
		}	

		if (Forces.Y == 0.0f && !States.ON_GROUND) {
			if (GroundUnderFeet()) {
				States.ON_GROUND = true;
				if (HasAuthority())
					CallTouchGround();
			}
		}else if (Forces.Y != 0.0f && States.ON_GROUND) {
			if (!GroundUnderFeet()) {
				States.ON_GROUND = false;
				if (HasAuthority())
					CallLeaveGround();
			}
		}

}

void ASPPawnCPP::CalculateMovement(float DeltaTime)
{	
	float CalculatedMovementSpeed;
		if (States.MOVE_LEFT) {
			if (GroundUnderFeet()) {
				CalculatedMovementSpeed = Attributes.MoveSpeed * StaticAttributes.MovementScale;
				if (Forces.X < CalculatedMovementSpeed * 2 && Forces.X > -CalculatedMovementSpeed) {
					Forces.X = -CalculatedMovementSpeed;
				}
			}
			else {
				CalculatedMovementSpeed = Attributes.MoveSpeed * StaticAttributes.AirMovementScale;
				if (Forces.X < CalculatedMovementSpeed * 2 && Forces.X > -CalculatedMovementSpeed) {
					Forces.X = -CalculatedMovementSpeed;
				}
			}
		}
		else if (States.MOVE_RIGHT) {
			if (GroundUnderFeet()) {
				CalculatedMovementSpeed = Attributes.MoveSpeed * StaticAttributes.MovementScale;
				if (Forces.X > -CalculatedMovementSpeed * 2 && Forces.X < CalculatedMovementSpeed) {
					Forces.X = CalculatedMovementSpeed;
				}
			}
			else {
				CalculatedMovementSpeed = Attributes.MoveSpeed * StaticAttributes.AirMovementScale;
				if (Forces.X > -CalculatedMovementSpeed * 2 && Forces.X < CalculatedMovementSpeed) {
					Forces.X = CalculatedMovementSpeed;
				}
			}
		}

		if (States.JUMP) {
			Forces.Y = Attributes.JumpPower;
		}
		else if (States.JUMP_LEFT_WALL) {

			Forces.Y = Attributes.JumpPower / StaticAttributes.WallJumpYModifier;
			if (Forces.X < Attributes.JumpPower / StaticAttributes.WallJumpXModifier) {
				Forces.X = Attributes.JumpPower / StaticAttributes.WallJumpXModifier;
			}
		}
		else if (States.JUMP_RIGHT_WALL) {

			Forces.Y = Attributes.JumpPower / StaticAttributes.WallJumpYModifier;
			if (Forces.X > -Attributes.JumpPower / StaticAttributes.WallJumpXModifier) {
				Forces.X = -Attributes.JumpPower / StaticAttributes.WallJumpXModifier;
			}
		}

		if (States.SIDE_DASH) {
			States.SIDE_DASH = false;
			if (WorkData.FacingRight) {
				if (Forces.X <= 0.0f && Forces.X >= -Attributes.Dash) {
					Forces.X = Attributes.Dash;
				}
				else if (Forces.X > 0) {
					Forces.X += Attributes.Dash;
				}
			}
			else {
				if (Forces.X >= 0.0f && Forces.X <= Attributes.Dash) {
					Forces.X = -Attributes.Dash;
				}
				else if (Forces.X < 0) {
					Forces.X -= Attributes.Dash;
				}
			}
		
			if (Forces.Y <= Attributes.Dash && Forces.Y >= -Attributes.Dash)
			{
				Forces.Y = 0.0f;
			}
		}
		else if (States.UP_DASH) {
			States.UP_DASH = false;
			if (Forces.X <= Attributes.Dash && Forces.X >= -Attributes.Dash) 
			{
				Forces.X = 0.0f;
			}

			if (Forces.Y <= 0.0f && Forces.Y >= -Attributes.Dash) 
			{
				Forces.Y = Attributes.Dash;
			}
			else if (Forces.Y > 0.0f) 
			{
				Forces.Y += Attributes.Dash;
			}
		}
		if (States.DOWN_DASH) {
			States.DOWN_DASH = false;
			if (Forces.X <= Attributes.Dash && Forces.X >= -Attributes.Dash)
			{
				Forces.X = 0.0f;
			}

			if (Forces.Y >= 0.0f && Forces.Y <= Attributes.Dash)
			{
				Forces.Y = -Attributes.Dash;
			}
			else if (Forces.Y < 0.0f)
			{
				Forces.Y -= Attributes.Dash;
			}
		}
		else if (States.SPOT_DODGE) {
			States.SPOT_DODGE = false;
		}

		if (WorkData.WasHit) {
			WorkData.WasHit = false;
			Forces.X += WorkData.HitForce.X * WorkData.HitForce.Z;
			Forces.Y += WorkData.HitForce.Y * WorkData.HitForce.Z;
		}

		if (WorkData.AddingForce) {
			WorkData.AddingForce = false;
			Forces.X += WorkData.AddForce.X * WorkData.AddForce.Z;
			Forces.Y += WorkData.AddForce.Y * WorkData.AddForce.Z;
		}
}

void ASPPawnCPP::CheckKeyStates()
{
	if (!HasAuthority()) {
		if (LastKeyStates.DEFENCE_KEY && !KeyStates.DEFENCE_KEY) {
			if(CanReleaseDefence())
			Server_ReleaseDefence();
		}
		else if (!LastKeyStates.DEFENCE_KEY && States.DEFENCE) {
			if (CanReleaseDefence())
				Server_ReleaseDefence();
		}
		else if (LastKeyStates.JUMP_KEY && !KeyStates.JUMP_KEY) {
			WorkData.CanJumpAgain = true;
			if (CanStopJump())
			Server_StopJump();

			float PingDelta = GetWorld()->GetFirstPlayerController()->PlayerState->Ping * 2.0f;
			PingDelta /= 1000.0f;
			GetWorldTimerManager().ClearTimer(WorkData.ClientTimer);
			GetWorldTimerManager().SetTimer(WorkData.ClientTimer, this, &ASPPawnCPP::StopJump, PingDelta, false);
		}
		else if (!LastKeyStates.JUMP_KEY && (States.JUMP || States.JUMP_LEFT_WALL || States.JUMP_RIGHT_WALL) ) {
			if (CanStopJump())
				Server_StopJump();
		}
		else if (LastKeyStates.SATTACK_KEY && !KeyStates.SATTACK_KEY) {
			if (CanReleaseStrongAttack())
			Server_ReleaseStrongAttack();
		}
		else if (!LastKeyStates.SATTACK_KEY && States.STRONG_ATTACK) {
			if (CanReleaseStrongAttack())
			Server_ReleaseStrongAttack();
		}
		else if (LastKeyStates.LEFT_KEY && !KeyStates.LEFT_KEY) {
			if (States.MOVE_LEFT && CanStopMove()) {
				Server_StopMove();
				
				float PingDelta = GetWorld()->GetFirstPlayerController()->PlayerState->Ping * 2.0f;
				PingDelta /= 1000.0f;
				GetWorldTimerManager().ClearTimer(WorkData.ClientTimer);
				GetWorldTimerManager().SetTimer(WorkData.ClientTimer, this, &ASPPawnCPP::StopMove, PingDelta, false);
			}
		}
		else if (!LastKeyStates.LEFT_KEY && States.MOVE_LEFT) {
			if (CanStopMove()) {
				Server_StopMove();
			}	
		}
		else if (LastKeyStates.RIGHT_KEY && !KeyStates.RIGHT_KEY) {
			if (States.MOVE_RIGHT && CanStopMove()) {
				Server_StopMove();
				float PingDelta = GetWorld()->GetFirstPlayerController()->PlayerState->Ping * 2.0f;
				PingDelta /= 1000.0f;
				GetWorldTimerManager().ClearTimer(WorkData.ClientTimer);
				GetWorldTimerManager().SetTimer(WorkData.ClientTimer, this, &ASPPawnCPP::StopMove, PingDelta, false);
			}
		}
		else if (!LastKeyStates.RIGHT_KEY && States.MOVE_RIGHT) {
			if (CanStopMove()) {
				Server_StopMove();
			}
		}
		else if (false && KeyStates.DEFENCE_KEY) {
			if (KeyStates.RIGHT_KEY || KeyStates.LEFT_KEY) {
				if(CanDash())
				Server_SideDash();
			}
			else if (KeyStates.UP_KEY) {
				if (CanDash())
				Server_UpDash();
			}
			else if (KeyStates.DOWN_KEY) {
				if (CanDash())
				Server_DownDash();
			}
			else {
				if (CanDefence())
				Server_Defence();
			}
		}
		else if (KeyStates.JUMP_KEY) {
			if (CanJump() && WorkData.CanJumpAgain) {
				Server_Jump();
				WorkData.CanJumpAgain = false;
			}
			
		}
		else if (KeyStates.LATTACK_KEY)
		{
			if (CanLightAttack())
			{
				if (KeyStates.UP_KEY) {
					Server_UpperLightAttack();
				}
				else if (KeyStates.DOWN_KEY) {
					Server_DownLightAttack();
				}
				else {
					Server_LightAttack();
				}
			}
		}
		else if (KeyStates.SATTACK_KEY)
		{
			if (CanStrongAttack()) {

				if (KeyStates.RIGHT_KEY || KeyStates.LEFT_KEY) {
					Server_SideStrongAttack();
				}
				else if (KeyStates.UP_KEY) {
					Server_UpperStrongAttack();
				}
				else if (KeyStates.DOWN_KEY) {
					Server_DownStrongAttack();
				}
				else {
					Server_StrongAttack();
				}
			}
		}
		else if (KeyStates.RIGHT_KEY) {
			if(!States.MOVE_RIGHT && CanMove())
			Server_MoveRight();
		}
		else if (KeyStates.LEFT_KEY) {
			if (!States.MOVE_LEFT && CanMove())
			Server_MoveLeft();
		}

		LastKeyStates = KeyStates;
	}
}

float ASPPawnCPP::ValuePerSecond(float value, float deltaTime)
{
	return value * (deltaTime / 1.0f);
}

void ASPPawnCPP::SetAttributes(FSPPawnAttributes new_attributes)
{
	Attributes = new_attributes;
	WorkData.CurrentDefence = Attributes.Defence;
	ClientCurrentDefence = WorkData.CurrentDefence;
}

void ASPPawnCPP::Move(bool right)
{
	if (HasAuthority()) {
		FVector CurrentPosition = GetActorLocation();
		FVector2D SendPosition;
		SendPosition.X = CurrentPosition.X;
		SendPosition.Y = CurrentPosition.Z;

		if (CanMove()) {
			if (right) {
				if(States.MOVE_LEFT) States.MOVE_LEFT = false;
				if(!States.MOVE_RIGHT) States.MOVE_RIGHT  = true;
				if (!WorkData.FacingRight) {
					FRotator rotation(0.0f, 0.0f, 0.0f);
					WorkData.FacingRight = true;
					animation->SetRelativeRotation(rotation, false);
				}
				Client_Move(SendPosition, 0);
			}
			else {
				if (!States.MOVE_LEFT) States.MOVE_LEFT = true;
				if (States.MOVE_RIGHT) States.MOVE_RIGHT = false;
				if (WorkData.FacingRight) {
					FRotator rotation(0.0f, 180.0f, 0.0f);
					WorkData.FacingRight = false;
					animation->SetRelativeRotation(rotation, false);

				}
				Client_Move(SendPosition, 1);
			}
			Actions.Move.ExecuteIfBound();
		}
	}
	else {
		if (right) {
			if (!WorkData.FacingRight) {
				FRotator rotation(0.0f, 0.0f, 0.0f);
				WorkData.FacingRight = true;
				animation->SetRelativeRotation(rotation, false);
			}
		}
		else {
			if (WorkData.FacingRight) {
				FRotator rotation(0.0f, 180.0f, 0.0f);
				WorkData.FacingRight = false;
				animation->SetRelativeRotation(rotation, false);
			}
		}
		Actions.Move.ExecuteIfBound();
	}
}

void ASPPawnCPP::StopMove()
{
	if (HasAuthority()) {
		if (CanStopMove()) {
			if (States.MOVE_LEFT) States.MOVE_LEFT = false;
			if (States.MOVE_RIGHT) States.MOVE_RIGHT = false;
			Actions.StopMove.ExecuteIfBound();
			Client_StopMove(GetSendPosition());
		}
	}
	else {
		if (States.MOVE_LEFT || States.MOVE_RIGHT) {
			if (States.MOVE_LEFT) States.MOVE_LEFT = false;
			if (States.MOVE_RIGHT) States.MOVE_RIGHT = false;
			Actions.StopMove.ExecuteIfBound();
		}
	}
}

void ASPPawnCPP::ResetActions(float delay_delta)
{
	if (HasAuthority()) {
		Actions.delay = delay_delta;
		GetWorldTimerManager().ClearTimer(DelayTimer);
		if (Actions.delay > 0.000f) {
			GetWorldTimerManager().SetTimer(DelayTimer, this, &ASPPawnCPP::CallDelayAction, Actions.delay, false);
		}
	}
}

bool ASPPawnCPP::Client_Dash_Validate(FVector2D n_Position, int index) {
	return true;
}

void ASPPawnCPP::Client_Dash_Implementation(FVector2D n_Position, int index)
{
	if (!HasAuthority()) {
		FVector CurrentPosition;
		CurrentPosition.X = n_Position.X;
		CurrentPosition.Z = n_Position.Y;
		CurrentPosition.Y = 0.0f;
		SetActorLocation(CurrentPosition, false);
		
		switch (index) {
		case 0:
			States.SIDE_DASH = true;
			break;
			
		case 1:
			States.UP_DASH = true;
			break;

		case 2:
			States.DOWN_DASH = true;
			break;

		case 3:
			States.SPOT_DODGE = true;
			break;
		}

		SetUpDash();
	}
}

bool ASPPawnCPP::CallDelayAction_Validate() {
	return true;
}

void ASPPawnCPP::CallDelayAction_Implementation()
{	
	if (HasAuthority()) {
		if (CanDelayAction()) {
			Actions.DelayAction.ExecuteIfBound();
		}
	}
	else {
		Actions.DelayAction.ExecuteIfBound();
	}
}

bool ASPPawnCPP::CallTouchGround_Validate() {
	return true;
}

void ASPPawnCPP::CallTouchGround_Implementation()
{
	Actions.TouchGround.ExecuteIfBound();
}

bool ASPPawnCPP::CallLeaveGround_Validate() {
	return true;
}

void ASPPawnCPP::CallLeaveGround_Implementation()
{
	Actions.LeaveGround.ExecuteIfBound();
}

void ASPPawnCPP::EndStun_Implementation()
{

}

void ASPPawnCPP::GetHit_Implementation(float hitstun, float damage, FVector knockback/*x/y are directions, z is force*/)
{
	if (HasAuthority()) {

		if (States.DEFENCE) {
			if (WorkData.CurrentDefence > damage) {
				WorkData.CurrentDefence -= damage;
				damage = damage / 10.0f;
			}
			else {
				WorkData.CurrentDefence = 0.0f;
				hitstun = hitstun * 1.25f;
			}
		}

		if (!States.DEFENCE || WorkData.CurrentDefence == 0.0f) {
			WorkData.WasHit = true;
			ClearStatesWhileHit();

			if (WorkData.HitStun < 0.06f) {
				WorkData.HitStun = hitstun
					+ ((hitstun * (float)WorkData.Injuries) / 100.0f); //Injuries adds to hitstun;
				WorkData.HitStun -= (WorkData.HitStun *(float)Attributes.Tenacity) / 100.0f; //Tenacity lowers hitstun
				ClientHitStun = WorkData.HitStun;
			}

			knockback.Z += knockback.Z / (100.0f / (WorkData.Injuries * 0.4f));
			WorkData.HitForce = knockback;
		}
	
		WorkData.Injuries += damage - (damage / (100.0f / ((float)Attributes.Tenacity * 0.5f)));

		//Replaced By ManageDefence
		//if (WorkData.CurrentDefence == 0.0f) {
		//	GetWorldTimerManager().ClearTimer(WorkData.DefenceTimer);
			//GetWorldTimerManager().SetTimer(WorkData.DefenceTimer, this, &ASPPawnCPP::ReplenishDefence, 1.0f, true);
		//}
	}
}

void ASPPawnCPP::DashEnd_Implementation()
{
}

bool ASPPawnCPP::FacingRight()
{
	if (WorkData.FacingRight)
		return true;
	else
	return false;
}

float ASPPawnCPP::StrongAttackMeter()
{
	return WorkData.StrongAttackMeter;
}

float ASPPawnCPP::CurrentDefence()
{
	return WorkData.CurrentDefence;
}

FVector2D ASPPawnCPP::GetSendPosition()
{
	FVector CurrentPosition = GetActorLocation();
	FVector2D SendPosition;
	SendPosition.X = CurrentPosition.X;
	SendPosition.Y = CurrentPosition.Z;

	return SendPosition;
}

bool ASPPawnCPP::IsStun()
{
	if (WorkData.HitStun > 0.0f) {
		return true;
	}
	return false;
}

bool ASPPawnCPP::CanMove() {
	if (HasAuthority()) {
		if (!IsStun() && !States.BUSY && States.CAN_MOVE && !States.DASH && !States.DEFENCE 
			&& !States.LIGHT_ATTACK && !States.STRONG_ATTACK) 
			return true;
		else 
			return false;
	}
	else {
		return true;
	}
	
}

bool ASPPawnCPP::CanStopMove() { 
	if (HasAuthority()) {
		if (States.MOVE_LEFT || States.MOVE_RIGHT)
			return true;
		else
			return false;
	}
	else {
		return true;
	}
}

bool ASPPawnCPP::CanDelayAction() {
	if (HasAuthority()) {
		if (!IsStun()) {
			return true;
		}
		else {
			return false;
		}
	}
	else {
		return true;
	}
}

bool ASPPawnCPP::CanJump() { 
	if (HasAuthority()) {
		if (!IsStun() && !States.BUSY && States.CAN_JUMP && !States.DEFENCE && !States.DASH
			&& !States.LIGHT_ATTACK && !States.STRONG_ATTACK && !States.JUMP &&
			!States.JUMP_LEFT_WALL && !States.JUMP_RIGHT_WALL) {
			if (WorkData.AirJumped < Attributes.AirJumpAmount || GroundUnderFeet()
				|| GroundNextToFeet(true) || GroundNextToFeet(false)) {
				return true;
			}
			else return false;
		}
		else
			return false;
	}
	else {
		return true;
	}
	
}

bool ASPPawnCPP::CanStopJump() { 
	if (HasAuthority()) {
		if (States.JUMP || States.JUMP_LEFT_WALL || States.JUMP_RIGHT_WALL) {
			return true;
		}
		return false;
	}
	else {
		return true;
	}
}

bool ASPPawnCPP::CanLightAttack() { 
	if (HasAuthority()) {
		if (!IsStun() && !States.BUSY && States.CAN_LIGHT_ATTACK && !States.DEFENCE && !States.DASH
			 && !States.STRONG_ATTACK)
			return true; 
		else 
			return false;
	}
	else {
		return true;
	}
}

bool ASPPawnCPP::CanStrongAttack() { 
	if (HasAuthority()) {
		if (!IsStun() && !States.BUSY && States.CAN_STRONG_ATTACK && !States.DEFENCE && !States.DASH
			&& !States.STRONG_ATTACK)
			return true;
		else
			return false;
	}
	else {
		return true;
	}
}

bool ASPPawnCPP::CanReleaseStrongAttack() { 
	if (HasAuthority()) {
		if (States.STRONG_ATTACK) {
			return true;
		}
		return false;
	}
	else {
		return true;
	}
}

bool ASPPawnCPP::CanDefence() { 
	if (HasAuthority()) {
		//You can not defend in air
		if (!IsStun() && !States.BUSY && States.CAN_DEFENCE && States.ON_GROUND && WorkData.CurrentDefence > 0.0f 
			&& !States.DEFENCE && !States.DASH && !States.LIGHT_ATTACK && !States.STRONG_ATTACK) {
			return true;
		}
		else 
			return false;
	}
	else {
		return true;
	}
}

bool ASPPawnCPP::CanReleaseDefence() {
	if (HasAuthority()){
		if (States.DEFENCE) {
			return true;
		}
		return false;
	}
	else {
		return true;
	}
}

bool ASPPawnCPP::CanDash()
{
	if (HasAuthority()) {
		if (!IsStun() && !States.BUSY && States.CAN_DASH && !States.DASH && !States.LIGHT_ATTACK 
			&& !States.STRONG_ATTACK && !States.DEFENCE) {
			return true;
		}
		else
			return false;
	}
	else {
		return true;
	}
}

void ASPPawnCPP::Client_Move_Implementation(FVector2D n_Position, int index)
{
	if (!HasAuthority()) {
		FVector CurrentPosition;
		CurrentPosition.X = n_Position.X;
		CurrentPosition.Z = n_Position.Y;
		CurrentPosition.Y = 0.0f;
		SetActorLocation(CurrentPosition, false);

		switch (index) {
		case 0:
			States.MOVE_LEFT = false;
			States.MOVE_RIGHT = true;
			Move(true);
			break;

		case 1:
			States.MOVE_RIGHT = false;
			States.MOVE_LEFT = true;
			Move(false);
			break;
		}

	}
}

bool ASPPawnCPP::Client_Move_Validate(FVector2D n_Position, int index)
{
	return true;
}

bool ASPPawnCPP::Client_StopMove_Validate(FVector2D n_Position) {
	return true;
}

void ASPPawnCPP::Client_StopMove_Implementation(FVector2D n_Position)
{
	if (!HasAuthority()) {
		if (States.MOVE_LEFT || States.MOVE_RIGHT) {

			FVector CurrentPosition;
			CurrentPosition.X = n_Position.X;
			CurrentPosition.Z = n_Position.Y;
			CurrentPosition.Y = 0.0f;
			SetActorLocation(CurrentPosition, false);
		
			GetWorldTimerManager().ClearTimer(WorkData.ClientTimer);
			StopMove();
		}
	}
}

void ASPPawnCPP::Client_Jump_Implementation(FVector2D n_Position, int index)
{
	if (!HasAuthority()) {
		FVector CurrentPosition;
		CurrentPosition.X = n_Position.X;
		CurrentPosition.Z = n_Position.Y;
		CurrentPosition.Y = 0.0f;
		SetActorLocation(CurrentPosition, false);

		switch (index) {
		case 0:
			States.JUMP = true;
			break;

		case 1:
			States.JUMP_RIGHT_WALL = true;
			break;

		case 2:
			States.JUMP_LEFT_WALL = true;
			break;
		}

		Jump();
	}
}

bool ASPPawnCPP::Client_Jump_Validate(FVector2D n_Position, int index)
{
	return true;
}
