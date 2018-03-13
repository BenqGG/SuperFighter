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
}

// Called when the game starts or when spawned
void ASPPawnCPP::BeginPlay()
{
	Super::BeginPlay();
	//TODO BIND GRAVITY DELEGATE delegate.AddDynamic(this, &YourClass::BindToDelegate);

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

void ASPPawnCPP::RepNot_UpdatePosition()
{
	SetActorLocation(CurrentPosition, false);
}

void ASPPawnCPP::HitPunch(bool FromClient, FVector2D ClientAxisPosition)
{
	if (HasAuthority()) {

		FSPHitBoxDetails HBDetails;
		FVector self_position;
		FVector self_bounds;
		FVector temp;


		HBDetails.ExistTime = 0.1f;
		HBDetails.FriendlyFire = true;
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
}

void ASPPawnCPP::Jump()
{	
	if (HasAuthority()) {
		if (GroundUnderFeet()) {
			States.JUMP = true;
			GetWorldTimerManager().SetTimer(WorkData.JumpTimer, this, &ASPPawnCPP::StopJump, 0.3f, false);
		}
		else if (GroundNextToFeet(true)) {
			States.JUMP_RIGHT_WALL = true;
			GetWorldTimerManager().SetTimer(WorkData.JumpTimer, this, &ASPPawnCPP::StopJump, 0.3f, false);
		}
		else if (GroundNextToFeet(false)) {
			States.JUMP_LEFT_WALL = true;
			GetWorldTimerManager().SetTimer(WorkData.JumpTimer, this, &ASPPawnCPP::StopJump, 0.3f, false);
		}
		else if (WorkData.AirJumped < Attributes.AirJumpAmount) {
			WorkData.AirJumped++;
			if (States.MOVE_LEFT) {
				States.JUMP_RIGHT_WALL = true;
				GetWorldTimerManager().SetTimer(WorkData.JumpTimer, this, &ASPPawnCPP::StopJump, 0.3f, false);
			}
			else if (States.MOVE_RIGHT) {
				States.JUMP_LEFT_WALL = true;
				GetWorldTimerManager().SetTimer(WorkData.JumpTimer, this, &ASPPawnCPP::StopJump, 0.3f, false);
			}
			else {
				States.JUMP = true;
				GetWorldTimerManager().SetTimer(WorkData.JumpTimer, this, &ASPPawnCPP::StopJump, 0.3f, false);
			}
		}
	}
	else {
		if (WorkData.AirJumped < Attributes.AirJumpAmount || GroundUnderFeet() || GroundNextToFeet(true) || GroundNextToFeet(false) ) {
			Server_Jump();
		}
	}
		
}

void ASPPawnCPP::StopJump()
{
	if (HasAuthority()) {
		if (States.JUMP) {
			States.JUMP = false;
			
		}
		if (States.JUMP_LEFT_WALL) {
			States.JUMP_LEFT_WALL = false;
		}
		if (States.JUMP_RIGHT_WALL) {
			States.JUMP_RIGHT_WALL = false;
		}
		GetWorldTimerManager().ClearTimer(WorkData.JumpTimer);
	}
	else {
		if (States.JUMP || States.JUMP_LEFT_WALL || States.JUMP_RIGHT_WALL) {
			Server_StopJump();
		}
	}
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
}

void ASPPawnCPP::Gravity(float DeltaTime)
{
	if (HasAuthority()) {
		Forces.Y -= ValuePerSecond(Attributes.Gravity, DeltaTime);
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
		CurrentPosition = GetActorLocation();
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
	}
}

float ASPPawnCPP::ValuePerSecond(float value, float deltaTime)
{
	return value * (deltaTime / 1.0f);
}

void ASPPawnCPP::SetAttributes(FSPPawnAttributes new_attributes)
{
	Attributes = new_attributes;
}

void ASPPawnCPP::Move(bool right)
{
	if (HasAuthority()) {
		if (right) {
			States.MOVE_LEFT = false;
			States.MOVE_RIGHT = true;
			WorkData.FacingRight = true;
		}
		else {
			States.MOVE_LEFT = true;
			States.MOVE_RIGHT = false;
			WorkData.FacingRight = false;
		}
	}
}

void ASPPawnCPP::StopMove()
{
	if (HasAuthority()) {
		States.MOVE_LEFT = false;
		States.MOVE_RIGHT = false;
	}
}



