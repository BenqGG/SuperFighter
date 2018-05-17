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
	WorkData.HitStun = 0.0f;
	WorkData.Injuries = 0;
	WorkData.ClientHitStun = 0.0f;
	WorkData.StrongAttackMeter = 0;
	WorkData.PossitionError = FVector(0.0f, 0.0f, 0.0f);
	WorkData.HitStun = 0.0f;
	WorkData.CurrentDefence = 0.0f;

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
		ApplyForces(DeltaTime);
		Friction(DeltaTime);
		Gravity(DeltaTime);
		CalculateMovement();
		CurrentPosition = GetActorLocation();
		Client_Forces = Forces;
		GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Yellow, FString::SanitizeFloat(GetController()->PlayerState->ExactPing));
		GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Yellow, FString::SanitizeFloat(WorkData.CurrentDefence));
	}
	else {
		ApplyForces(DeltaTime);
		FixPossitionError();
		Friction(DeltaTime);
		Gravity(DeltaTime);
		CalculateMovement();
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
	FVector current_location = GetActorLocation();
	WorkData.PossitionError.X = CurrentPosition.X - current_location.X;
	WorkData.PossitionError.Z = CurrentPosition.Z - current_location.Z;
}

void ASPPawnCPP::RepNot_UpdateHitStun()
{
	APlayerState*check= UGameplayStatics::GetPlayerController(GetWorld(), 0)->PlayerState;
	if (IsValid(check) && ClientHitStun > 0.0f){
		ClientHitStun -= (check->Ping / 1000.0f)* 4.0f;
	}
}


void ASPPawnCPP::HitPunch(bool FromClient, FVector2D ClientAxisPosition)
{
	if (HasAuthority()) {

		FSPHitBoxDetails HBDetails;
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
	}
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
			else if (AxisX > 0.0f && AxisX > 0.2f && !States.MOVE_RIGHT) {
				Move(true);
			}
			else if (AxisX < 0.0f && AxisX < -0.2f && !States.MOVE_LEFT) {
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
	DOREPLIFETIME(ASPPawnCPP, CurrentPosition);
	DOREPLIFETIME(ASPPawnCPP, States);
	DOREPLIFETIME(ASPPawnCPP, Attributes);
	DOREPLIFETIME(ASPPawnCPP, WorkData);
	DOREPLIFETIME(ASPPawnCPP, Client_Forces);
	DOREPLIFETIME(ASPPawnCPP, StaticAttributes);
	DOREPLIFETIME(ASPPawnCPP, ClientHitStun);
}


void ASPPawnCPP::Jump()
{	
	if (HasAuthority()) {
			if (CanJump()) {
				if (GroundUnderFeet()) {
					if(!States.JUMP) States.JUMP = true;
					GetWorldTimerManager().ClearTimer(WorkData.JumpTimer);
					GetWorldTimerManager().SetTimer(WorkData.JumpTimer, this, &ASPPawnCPP::StopJump, 0.3f, false);
					Actions.Jump.ExecuteIfBound();
				}
				else if (GroundNextToFeet(true)) {
					if (!States.JUMP_RIGHT_WALL) States.JUMP_RIGHT_WALL = true;
					GetWorldTimerManager().ClearTimer(WorkData.JumpTimer);
					GetWorldTimerManager().SetTimer(WorkData.JumpTimer, this, &ASPPawnCPP::StopJump, 0.3f, false);
					Actions.Jump.ExecuteIfBound();
				}
				else if (GroundNextToFeet(false)) {
					if(!States.JUMP_LEFT_WALL) States.JUMP_LEFT_WALL = true;
					GetWorldTimerManager().ClearTimer(WorkData.JumpTimer);
					GetWorldTimerManager().SetTimer(WorkData.JumpTimer, this, &ASPPawnCPP::StopJump, 0.3f, false);
					Actions.Jump.ExecuteIfBound();
				}
				else if (WorkData.AirJumped < Attributes.AirJumpAmount) {
					WorkData.AirJumped++;
					if (States.MOVE_LEFT) {
						if(!States.JUMP_RIGHT_WALL) States.JUMP_RIGHT_WALL = true;
						GetWorldTimerManager().ClearTimer(WorkData.JumpTimer);
						GetWorldTimerManager().SetTimer(WorkData.JumpTimer, this, &ASPPawnCPP::StopJump, 0.3f, false);
						Actions.Jump.ExecuteIfBound();
					}
					else if (States.MOVE_RIGHT) {
						if(!States.JUMP_LEFT_WALL) States.JUMP_LEFT_WALL = true;
						GetWorldTimerManager().ClearTimer(WorkData.JumpTimer);
						GetWorldTimerManager().SetTimer(WorkData.JumpTimer, this, &ASPPawnCPP::StopJump, 0.3f, false);
						Actions.Jump.ExecuteIfBound();
					}
					else {
						if(!States.JUMP) States.JUMP = true;
						GetWorldTimerManager().ClearTimer(WorkData.JumpTimer);
						GetWorldTimerManager().SetTimer(WorkData.JumpTimer, this, &ASPPawnCPP::StopJump, 0.3f, false);
						Actions.Jump.ExecuteIfBound();
					}
			}
		}
	}
	else {
		if (CanJump()) {
			Server_Jump();
		}
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
		if (CanStopJump()) {
			Server_StopJump();
		}
	}
}

void ASPPawnCPP::LightAttack(int index)
{
	if (HasAuthority()) {
		if (CanLightAttack()) {
			if (index == 0) {
				FVector2D CurrentAxis = AxisPosition();
				if ((CurrentAxis.X == 0.0f && CurrentAxis.Y == 0.0f) || (abs(CurrentAxis.X) > abs(CurrentAxis.Y))) {
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
			if ((CurrentAxis.X == 0.0f && CurrentAxis.Y == 0.0f) || (abs(CurrentAxis.X) > abs(CurrentAxis.Y))) {
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
				if (CurrentAxis.X == 0.0f && CurrentAxis.Y == 0.0f) {
					if (States.ON_GROUND)
						Actions.StrongAttack.ExecuteIfBound();
					else
						Actions.AirStrongAttack.ExecuteIfBound();
				}
				else if (abs(CurrentAxis.X) > abs(CurrentAxis.Y)) {
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
			if (CurrentAxis.X == 0.0f && CurrentAxis.Y == 0.0f) {
					Server_StrongAttack();
			}
			else if (abs(CurrentAxis.X) > abs(CurrentAxis.Y)) {
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
		if (index == 0) {
			FVector2D CurrentAxis = AxisPosition();
			if (CurrentAxis.X == 0.0f && CurrentAxis.Y == 0.0f) {
				if (CanDefence()) 
				{	
					SetUpDefence();
					Actions.Defence.ExecuteIfBound();
				}
			}
			else if (abs(CurrentAxis.X) >= abs(CurrentAxis.Y) ) {
				if (CanDash()) {
					States.SIDE_DASH = true;
					SetUpDash();

					if (States.ON_GROUND)
						Actions.Dash.ExecuteIfBound();
					else
						Actions.AirDash.ExecuteIfBound();
				}
			}
			else {
				if (CurrentAxis.Y > 0.0f) {
					if (CanDash()) {
						States.UP_DASH = true;
						SetUpDash();

						if (States.ON_GROUND)
							Actions.Dash.ExecuteIfBound();
						else
							Actions.AirDash.ExecuteIfBound();
					}
				}
				else {
					if (CanDash()) {
						if (States.ON_GROUND) {
							States.SPOT_DODGE = true;
							SetUpDash();

							Actions.SpotDodge.ExecuteIfBound();
						}
							
						else {
							States.DOWN_DASH = true;
							SetUpDash();

							Actions.AirDash.ExecuteIfBound();
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
				if (index == 2) States.SIDE_DASH = true;
				else States.UP_DASH = true;

				SetUpDash();

				if(States.ON_GROUND)
					Actions.Dash.ExecuteIfBound();
				else
					Actions.AirDash.ExecuteIfBound();
			}
		}
		else if (index == 4) {
			if (CanDash()) {
				if (States.ON_GROUND) {
					States.SPOT_DODGE = true;
					SetUpDash();
					Actions.SpotDodge.ExecuteIfBound();
				}
				else {
					States.DOWN_DASH = true;
					SetUpDash();
					Actions.AirDash.ExecuteIfBound();
				}
					
			}
		}
	}
	else {
		FVector2D CurrentAxis = AxisPosition();
		if (CurrentAxis.X == 0.0f && CurrentAxis.Y == 0.0f) {
			if (CanDefence()) {
				Server_Defence();
			}
		}
		else if (abs(CurrentAxis.X) >= abs(CurrentAxis.Y) && CanDash()) {
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
			
			if (WorkData.CurrentDefence < Attributes.Defence) {
				GetWorldTimerManager().SetTimer(WorkData.DefenceTimer, this, &ASPPawnCPP::ReplenishDefence, 1.0f, true);
			}
		}
	}
	else {
		if (CanReleaseDefence()) {
			Server_ReleaseDefence();
		}
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

void ASPPawnCPP::
ChangeAnimation(FSPAnimationDetails details)
{
	collision_box->SetBoxExtent(FVector(details.CollisionBox.X, 10, details.CollisionBox.Y), true);
	hit_box->SetBoxExtent(FVector(details.HitBox.X, 10, details.HitBox.Y), true);
	animation->SetFlipbook(details.Flipbook);
}

void ASPPawnCPP::Friction(float DeltaTime)
{
	if(HasAuthority()){
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
	else {
		if (Client_Forces.X != 0.0f) {
			float current_friction;
			if (GroundUnderFeet()) {
				current_friction = Attributes.Friction;
			}
			else {
				current_friction = Attributes.AirFriction;
			}
			Client_Forces.X > 0 ? current_friction : current_friction *= -1;
			Client_Forces.X -= ValuePerSecond(current_friction, DeltaTime);
		}
	}
}

void ASPPawnCPP::Gravity(float DeltaTime)
{
	if (HasAuthority()) {
		Forces.Y -= ValuePerSecond(Attributes.Gravity, DeltaTime);
	}
	else {
		Client_Forces.Y -= ValuePerSecond(Attributes.Gravity, DeltaTime);
	}
	
}

void ASPPawnCPP::FixPossitionError()
{
	FVector current_position = GetActorLocation();

	if ( (WorkData.PossitionError.X > 0.0f && WorkData.PossitionError.X <= 10.0f ) || (WorkData.PossitionError.X > 20.0f)) {
		current_position.X += WorkData.PossitionError.X;
		WorkData.PossitionError.X = 0.0f;
	}
	else if (WorkData.PossitionError.X > 10.0f && WorkData.PossitionError.X <= 20.0f) {
		current_position.X += 10.0f;
		WorkData.PossitionError.X -= 10.0f;
	}
	else if ((WorkData.PossitionError.X < 0.0f && WorkData.PossitionError.X >= -10.0f) || (WorkData.PossitionError.X < -20.0f)) {
		current_position.X += WorkData.PossitionError.X;
		WorkData.PossitionError.X = 0.0f;
	}
	else if (WorkData.PossitionError.X < -10.0f && WorkData.PossitionError.X >= -20.0f) {
		current_position.X += 10.0f;
		WorkData.PossitionError.X += 10.0f;
	}

	if ((WorkData.PossitionError.Z > 0.0f && WorkData.PossitionError.Z <= 10.0f) || (WorkData.PossitionError.Z > 20.0f)) {
		current_position.Z += WorkData.PossitionError.Z;
		WorkData.PossitionError.Z = 0.0f;
	}
	else if (WorkData.PossitionError.X > 10.0f && WorkData.PossitionError.X <= 20.0f) {
		current_position.Z += 10.0f;
		WorkData.PossitionError.Z -= 10.0f;
	}
	else if ((WorkData.PossitionError.Z < 0.0f && WorkData.PossitionError.Z >= -10.0f) || (WorkData.PossitionError.Z < -20.0f)) {
		current_position.Z += WorkData.PossitionError.Z;
		WorkData.PossitionError.Z = 0.0f;
	}
	else if (WorkData.PossitionError.Z < -10.0f && WorkData.PossitionError.Z >= -20.0f) {
		current_position.Z += 10.0f;
		WorkData.PossitionError.Z += 10.0f;
	}
	

	SetActorLocation(current_position, false);
}

void ASPPawnCPP::UpgradeStrongAttackMeter()
{
	WorkData.StrongAttackMeter++;
}

void ASPPawnCPP::ClearStatesWhileHit()
{
	if (HasAuthority()) {
		if(States.MOVE_RIGHT)States.MOVE_RIGHT = false;
		if (States.MOVE_LEFT)States.MOVE_LEFT = false;
		if (States.JUMP)States.JUMP = false;
		if (States.JUMP_LEFT_WALL)States.JUMP_LEFT_WALL = false;
		if (States.JUMP_RIGHT_WALL)States.JUMP_RIGHT_WALL = false;
	
	
		if (!States.CAN_MOVE)States.CAN_MOVE = true;
		if (!States.CAN_JUMP)States.CAN_JUMP = true;
		if (!States.CAN_LIGHT_ATTACK)States.CAN_LIGHT_ATTACK = true;
		if (!States.CAN_STRONG_ATTACK)States.CAN_STRONG_ATTACK = true;
		if (!States.CAN_DEFENCE)States.CAN_DEFENCE = true;
		
		if (States.DEFENCE)States.DEFENCE = false;
		if (States.STRONG_ATTACK)States.STRONG_ATTACK = false;
		if (States.LIGHT_ATTACK)States.LIGHT_ATTACK = false;
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

		GetWorldTimerManager().ClearTimer(WorkData.DefenceTimer);
		GetWorldTimerManager().SetTimer(WorkData.DefenceTimer, this, &ASPPawnCPP::UseDefence, 1.0f, true);
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

		GetWorldTimerManager().ClearTimer(WorkData.DefenceTimer);
	}
}

void ASPPawnCPP::UseDefence()
{
	if (HasAuthority()) {
		WorkData.CurrentDefence -= Attributes.Defence / 20.0f;
		if (WorkData.CurrentDefence <= 0.0f) {
			WorkData.CurrentDefence = 0.0f;
			GetWorldTimerManager().ClearTimer(WorkData.DefenceTimer);
			ReleaseDefence();
		}
	}
}

void ASPPawnCPP::ReplenishDefence()
{
	if (HasAuthority()) {
		if (WorkData.CurrentDefence < Attributes.Defence) {
			WorkData.CurrentDefence += Attributes.Defence / 10.0f;
			if (WorkData.CurrentDefence >= Attributes.Defence) {
				WorkData.CurrentDefence = Attributes.Defence;
				GetWorldTimerManager().ClearTimer(WorkData.DefenceTimer);
			}
		}
		else {
			GetWorldTimerManager().ClearTimer(WorkData.DefenceTimer);
		}
	}
}

void ASPPawnCPP::SetUpDash()
{
	if (HasAuthority()) {
		
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

		if (States.CAN_MOVE)				States.CAN_MOVE = false;
		if (States.CAN_JUMP)				States.CAN_JUMP = false;
		if (States.CAN_LIGHT_ATTACK)		States.CAN_LIGHT_ATTACK = false;
		if (States.CAN_STRONG_ATTACK)		States.CAN_STRONG_ATTACK = false;
		if (States.CAN_DEFENCE)				States.CAN_DEFENCE = false;
		if (States.CAN_DASH)				States.CAN_DASH = false;

		if (States.SPOT_DODGE) {
			GetWorldTimerManager().SetTimer(WorkData.DashTimer, this, &ASPPawnCPP::StopDash, 0.2f, false);
		}
		else {
			GetWorldTimerManager().SetTimer(WorkData.DashTimer, this, &ASPPawnCPP::StopDash, 0.5f, false);
		}
	}
}

void ASPPawnCPP::StopDash()
{
	if (HasAuthority()) {
		if (States.DASH) States.DASH = false;

		if (!States.CAN_MOVE)				States.CAN_MOVE = true;
		if (!States.CAN_JUMP)				States.CAN_JUMP = true;
		if (!States.CAN_LIGHT_ATTACK)		States.CAN_LIGHT_ATTACK = true;
		if (!States.CAN_STRONG_ATTACK)		States.CAN_STRONG_ATTACK = true;
		if (!States.CAN_DEFENCE)			States.CAN_DEFENCE = true;

		if (States.CAN_DASH) States.CAN_DASH = false;
		
		GetWorldTimerManager().ClearTimer(WorkData.DashTimer);
		GetWorldTimerManager().SetTimer(WorkData.DashTimer, this, &ASPPawnCPP::DashColdown, 1.0f, false);

	}
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
		if(States.LIGHT_ATTACK) States.LIGHT_ATTACK = false;
		

		if(States.CAN_MOVE) States.CAN_MOVE = false;
		if(States.CAN_JUMP) States.CAN_JUMP = false;
		if(States.CAN_LIGHT_ATTACK) States.CAN_LIGHT_ATTACK = false;
		if(States.CAN_STRONG_ATTACK) States.CAN_STRONG_ATTACK = false;
		if(States.CAN_DEFENCE) States.CAN_DEFENCE = false;
		if(States.CAN_DASH) States.CAN_DASH  = false;
		
		WorkData.StrongAttackMeter = 0;
		GetWorldTimerManager().ClearTimer(WorkData.StrongAttackTimer);
		GetWorldTimerManager().SetTimer(WorkData.StrongAttackTimer, this, &ASPPawnCPP::UpgradeStrongAttackMeter, 0.1f, true);
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

	if (HasAuthority()) {
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
				Actions.TouchGround.ExecuteIfBound();
			}
		}else if (Forces.Y != 0.0f && States.ON_GROUND) {
			if (!GroundUnderFeet()) {
				States.ON_GROUND = false;
				Actions.LeaveGround.ExecuteIfBound();
			}
		}
	}
	else {
		if (Client_Forces.X != 0.0f) {
			current_location = GetActorLocation();
			current_location.X += Client_Forces.X * (DeltaTime / 1.0f);
			SetActorLocation(current_location, true, nullptr);
		}
		if (Client_Forces.Y != 0.0f) {
			current_location = GetActorLocation();
			current_location.Z += Client_Forces.Y * (DeltaTime / 1.0f);
			SetActorLocation(current_location, true, nullptr);
		}
	}
}

void ASPPawnCPP::CalculateMovement()
{
	if (HasAuthority()) {
			
		if (States.MOVE_LEFT) {
			if (GroundUnderFeet()) {
				if (Forces.X == 0) {
					Forces.X -= Attributes.MoveSpeed * StaticAttributes.MovementScale;
				}
				else if (Forces.X < 0 && Forces.X >(-Attributes.MoveSpeed * StaticAttributes.MovementScale)) {
					Forces.X = -Attributes.MoveSpeed * StaticAttributes.MovementScale;
				}
				else if (Forces.X > 0 && Forces.X < (Attributes.MoveSpeed * StaticAttributes.MovementScale)) {
					Forces.X = -Attributes.MoveSpeed * StaticAttributes.MovementScale;
				}
			}
			else {
				if (Forces.X == 0) {
					Forces.X -= Attributes.MoveSpeed * StaticAttributes.AirMovementScale;
				}
				else if (Forces.X < 0 && Forces.X >(-Attributes.MoveSpeed * StaticAttributes.AirMovementScale)) {
					Forces.X = -Attributes.MoveSpeed * StaticAttributes.AirMovementScale;
				}
				else if (Forces.X > 0 && Forces.X < (Attributes.MoveSpeed * StaticAttributes.AirMovementScale)) {
					Forces.X = -Attributes.MoveSpeed * StaticAttributes.AirMovementScale;
				}
			}
		}
		else if (States.MOVE_RIGHT) {
			if (GroundUnderFeet()) {
				if (Forces.X == 0) {
					Forces.X = Attributes.MoveSpeed * StaticAttributes.MovementScale;
				}
				else if (Forces.X > 0 && Forces.X < (Attributes.MoveSpeed * StaticAttributes.MovementScale)) {
					Forces.X = Attributes.MoveSpeed * StaticAttributes.MovementScale;
				}
				else if (Forces.X < 0 && Forces.X >(-Attributes.MoveSpeed * StaticAttributes.MovementScale)) {
					Forces.X = Attributes.MoveSpeed * StaticAttributes.MovementScale;
				}
			}
			else {
				if (Forces.X == 0) {
					Forces.X = Attributes.MoveSpeed * StaticAttributes.AirMovementScale;
				}
				else if (Forces.X > 0 && Forces.X < (Attributes.MoveSpeed * StaticAttributes.AirMovementScale)) {
					Forces.X = Attributes.MoveSpeed * StaticAttributes.AirMovementScale;
				}
				else if (Forces.X < 0 && Forces.X >(-Attributes.MoveSpeed * StaticAttributes.AirMovementScale)) {
					Forces.X = Attributes.MoveSpeed * StaticAttributes.AirMovementScale;
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
				if (Forces.X > (-Attributes.Dash * 2.0f) && Forces.X <= 0.0f) {
						Forces.X = Attributes.Dash / 1.5f;
				}
				else {
					Forces.X += Attributes.Dash / 1.5f;
				}
			}
			else {
				if (Forces.X < (Attributes.Dash * 2.0f) && Forces.X >= 0.0f) {
					Forces.X = -Attributes.Dash / 1.5f;
				}
				else {
					Forces.X -= Attributes.Dash / 1.5f;
				}
			}
		}
		else if (States.UP_DASH) {
			States.UP_DASH = false;
			if (Forces.Y >(-Attributes.Dash * 2.0f) && Forces.Y <= 0.0f) {
				Forces.Y = Attributes.Dash;
			}
			else {
				Forces.Y += Attributes.Dash;
			}
		}
		if (States.DOWN_DASH) {
			States.DOWN_DASH = false;
			if (Forces.Y < (Attributes.Dash * 2.0f) && Forces.Y >= 0.0f) {
				Forces.Y = -Attributes.Dash;
			}
			else {
				Forces.Y -= Attributes.Dash;
			}
		}
		else if (States.SPOT_DODGE) {
			States.SPOT_DODGE = false;
		}

	}
	else {
		if (States.MOVE_LEFT) {
			if (GroundUnderFeet()) {
				if (Client_Forces.X == 0) {
					Client_Forces.X -= Attributes.MoveSpeed * StaticAttributes.MovementScale;
				}
				else if (Client_Forces.X < 0 && Client_Forces.X >(-Attributes.MoveSpeed * StaticAttributes.MovementScale)) {
					Client_Forces.X = -Attributes.MoveSpeed * StaticAttributes.MovementScale;
				}
				else if (Client_Forces.X > 0 && Client_Forces.X < (Attributes.MoveSpeed * StaticAttributes.MovementScale)) {
					Client_Forces.X = -Attributes.MoveSpeed * StaticAttributes.MovementScale;
				}
			}
			else {
				if (Client_Forces.X == 0) {
					Client_Forces.X -= Attributes.MoveSpeed * StaticAttributes.AirMovementScale;
				}
				else if (Client_Forces.X < 0 && Client_Forces.X >(-Attributes.MoveSpeed * StaticAttributes.AirMovementScale)) {
					Client_Forces.X = -Attributes.MoveSpeed * StaticAttributes.AirMovementScale;
				}
				else if (Client_Forces.X > 0 && Client_Forces.X < (Attributes.MoveSpeed * StaticAttributes.AirMovementScale)) {
					Client_Forces.X = -Attributes.MoveSpeed * StaticAttributes.AirMovementScale;
				}
			}
		}
		else if (States.MOVE_RIGHT) {
			if (GroundUnderFeet()) {
				if (Client_Forces.X == 0) {
					Client_Forces.X = Attributes.MoveSpeed * StaticAttributes.MovementScale;
				}
				else if (Client_Forces.X > 0 && Client_Forces.X < (Attributes.MoveSpeed * StaticAttributes.MovementScale)) {
					Client_Forces.X = Attributes.MoveSpeed * StaticAttributes.MovementScale;
				}
				else if (Client_Forces.X < 0 && Client_Forces.X >(-Attributes.MoveSpeed * StaticAttributes.MovementScale)) {
					Client_Forces.X = Attributes.MoveSpeed * StaticAttributes.MovementScale;
				}
			}
			else {
				if (Client_Forces.X == 0) {
					Client_Forces.X = Attributes.MoveSpeed * StaticAttributes.AirMovementScale;
				}
				else if (Client_Forces.X > 0 && Client_Forces.X < (Attributes.MoveSpeed * StaticAttributes.AirMovementScale)) {
					Client_Forces.X = Attributes.MoveSpeed * StaticAttributes.AirMovementScale;
				}
				else if (Client_Forces.X < 0 && Client_Forces.X >(-Attributes.MoveSpeed * StaticAttributes.AirMovementScale)) {
					Client_Forces.X = Attributes.MoveSpeed * StaticAttributes.AirMovementScale;
				}
			}
		}

		if (States.JUMP) {
			Client_Forces.Y = Attributes.JumpPower;
		}
		else if (States.JUMP_LEFT_WALL) {

			Client_Forces.Y = Attributes.JumpPower / StaticAttributes.WallJumpYModifier;
			if (Client_Forces.X < Attributes.JumpPower / StaticAttributes.WallJumpXModifier) {
				Client_Forces.X = Attributes.JumpPower / StaticAttributes.WallJumpXModifier;
			}
		}
		else if (States.JUMP_RIGHT_WALL) {

			Client_Forces.Y = Attributes.JumpPower / StaticAttributes.WallJumpYModifier;
			if (Client_Forces.X > -Attributes.JumpPower / StaticAttributes.WallJumpXModifier) {
				Client_Forces.X = -Attributes.JumpPower / StaticAttributes.WallJumpXModifier;
			}
		}

		if (States.SIDE_DASH) {
			States.SIDE_DASH = false;
			if (WorkData.FacingRight) {
				if (Forces.X > (-Attributes.Dash * 2.0f) && Forces.X <= 0.0f) {
						Forces.X = Attributes.Dash;
				}
				else {
					Forces.X += Attributes.Dash;
				}
			}
			else {
				if (Forces.X < (Attributes.Dash * 2.0f) && Forces.X >= 0.0f) {
					Forces.X = -Attributes.Dash;
				}
				else {
					Forces.X -= Attributes.Dash;
				}
			}
		}
		else if (States.UP_DASH) {
			States.UP_DASH = false;
			if (Forces.Y >(-Attributes.Dash * 2.0f) && Forces.Y <= 0.0f) {
				Forces.Y = Attributes.Dash;
			}
			else {
				Forces.Y += Attributes.Dash;
			}
		}
		if (States.DOWN_DASH) {
			States.DOWN_DASH = false;
			if (Forces.Y < (Attributes.Dash * 2.0f) && Forces.Y >= 0.0f) {
				Forces.Y = -Attributes.Dash;
			}
			else {
				Forces.Y -= Attributes.Dash;
			}
		}
		else if (States.SPOT_DODGE) {
			States.SPOT_DODGE = false;
		}
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
}

void ASPPawnCPP::Move(bool right)
{
	if (HasAuthority()) {
		if (CanMove()) {
			if (right) {
				if(States.MOVE_LEFT) States.MOVE_LEFT = false;
				if(!States.MOVE_RIGHT) States.MOVE_RIGHT  = true;
				if(!WorkData.FacingRight)WorkData.FacingRight = true;
			}
			else {
				if (!States.MOVE_LEFT) States.MOVE_LEFT = true;
				if (States.MOVE_RIGHT) States.MOVE_RIGHT = false;
				if (WorkData.FacingRight)WorkData.FacingRight = false;
			}
			Actions.Move.ExecuteIfBound();
		}
	}
}

void ASPPawnCPP::StopMove()
{
	if (HasAuthority()) {
		if (CanStopMove()) {
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
		if (Actions.delay > 0.0f) {
			GetWorldTimerManager().SetTimer(DelayTimer, this, &ASPPawnCPP::CallDelayAction, Actions.delay, false);
		}
	}
}

void ASPPawnCPP::CallDelayAction()
{	
	if (HasAuthority()) {
		if (CanDelayAction()) {
			Actions.DelayAction.ExecuteIfBound();
		}
	}
}

void ASPPawnCPP::GetHit(float hitstun, float damage, FVector knockback/*x/y are directions, z is force*/)
{
	if (HasAuthority()) {
		float injuries = ((damage * Attributes.Tenacity) / 100.0f);
		if (States.DEFENCE) {
			if (WorkData.CurrentDefence >= injuries) {
				WorkData.CurrentDefence -= injuries;
				injuries = injuries / 10.0f;
			}
			else {
				WorkData.CurrentDefence = 0.0f;
				WorkData.HitStun = hitstun
					+ ((hitstun * (float)WorkData.Injuries) / 100.0f); //Injuries adds to hitstun;
				WorkData.HitStun -= (WorkData.HitStun *(float)Attributes.Tenacity) / 100.0f; //Tenacity lowers hitstun
				WorkData.HitStun *= 3.0f;
				ClientHitStun = WorkData.HitStun;
				ClearStatesWhileHit();
			}
			if (WorkData.CurrentDefence < Attributes.Defence) {
				GetWorldTimerManager().ClearTimer(WorkData.DefenceTimer);
				GetWorldTimerManager().SetTimer(WorkData.DefenceTimer, this, &ASPPawnCPP::ReplenishDefence, 1.0f, true);
			}
		}
		else {
			if (WorkData.HitStun < 0.03f) {
				WorkData.HitStun = hitstun
					+ ((hitstun * (float)WorkData.Injuries) / 100.0f); //Injuries adds to hitstun;
				WorkData.HitStun -= (WorkData.HitStun *(float)Attributes.Tenacity) / 100.0f; //Tenacity lowers hitstun
				ClientHitStun = WorkData.HitStun;
				ClearStatesWhileHit();
			}
		}
		WorkData.Injuries += injuries;
	}
}

bool ASPPawnCPP::IsStun()
{
	if (HasAuthority()) {
		if (WorkData.HitStun > 0.0f) {
			return true;
		}
		return false;
	}
	else {
		if (ClientHitStun > 0.0f) {
			return true;
		}
		return false;
	}
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
		if (!IsStun() && !States.BUSY && States.CAN_MOVE && !States.DASH && !States.DEFENCE
			&& !States.LIGHT_ATTACK && !States.STRONG_ATTACK) 
			return true;
		else 
			return false;
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
		if (States.MOVE_LEFT || States.MOVE_RIGHT)
			return true; 
		else
			return false;
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
		if (!IsStun()) {
			return true;
		}
		else {
			return false;
		}
	}
}

bool ASPPawnCPP::CanJump() { 
	if (HasAuthority()) {
		if (!IsStun() && !States.BUSY && States.CAN_JUMP && !States.DEFENCE && !States.DASH
			&& !States.LIGHT_ATTACK && !States.STRONG_ATTACK) {
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
		if (!IsStun() && !States.BUSY && States.CAN_JUMP && !States.DEFENCE && !States.DASH
			&& !States.LIGHT_ATTACK && !States.STRONG_ATTACK) {
			if (WorkData.AirJumped < Attributes.AirJumpAmount) {
				return true;
			}
			else return false;
		}
		else
			return false;
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
		if (States.JUMP || States.JUMP_LEFT_WALL || States.JUMP_RIGHT_WALL) {
			return true;
		}
		return false;
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
		if (!IsStun() && !States.BUSY && States.CAN_LIGHT_ATTACK && !States.DEFENCE && !States.DASH
			&& !States.STRONG_ATTACK)
			return true;
		else
			return false;
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
		if (!IsStun() && !States.BUSY && States.CAN_STRONG_ATTACK && !States.DEFENCE && !States.DASH
			&& !States.STRONG_ATTACK)
			return true;
		else
			return false;
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
		if (States.STRONG_ATTACK) {
			return true;
		}
		return false;
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
		if (!IsStun() && !States.BUSY && States.CAN_DEFENCE && !States.DEFENCE 
			&& !States.DASH && !States.LIGHT_ATTACK && !States.STRONG_ATTACK) {
			return true;
		}
		else
			return false;
	}
}

bool ASPPawnCPP::CanReleaseDefence() {
	if (HasAuthority()) {
		if (States.DEFENCE) {
			return true;
		}
		return false;
	}
	else {
		if (States.DEFENCE) {
			return true;
		}
		return false;
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
		if (!IsStun() && !States.BUSY && States.CAN_DASH && !States.DASH && !States.LIGHT_ATTACK
			&& !States.STRONG_ATTACK && !States.DEFENCE) {
			return true;
		}
		else
			return false;
	}
}
