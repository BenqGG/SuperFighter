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

	ApplyForces(DeltaTime);
	Friction(DeltaTime);
	CalculateMovement();

	if (WorkData.IsLocal) {

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

void ASPPawnCPP::GravityDelegeteBind(const float Pull)
{
	//Pull means how much you should lower y force this frame, pull is calculated on gravity object
	Forces.Y -= Pull;
}

void ASPPawnCPP::Jump()
{	
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

void ASPPawnCPP::StopJump()
{
	if (States.JUMP) {
		States.JUMP = false;
	}
	if (States.JUMP_LEFT_WALL) {
		States.JUMP_LEFT_WALL = false;
	}
	if (States.JUMP_RIGHT_WALL) {
		States.JUMP_RIGHT_WALL = false;
	}
}

void ASPPawnCPP::Hit(float forceX, float forceY)
{
	Forces.X = forceX;
	Forces.Y = forceY;
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
}

void ASPPawnCPP::CalculateMovement()
{
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
	if (right) {
		States.MOVE_LEFT = false;
		States.MOVE_RIGHT = true;
	}
	else {
		States.MOVE_LEFT = true;
		States.MOVE_RIGHT = false;
	}
}

void ASPPawnCPP::StopMove()
{
	States.MOVE_LEFT = false;
	States.MOVE_RIGHT = false;
}



