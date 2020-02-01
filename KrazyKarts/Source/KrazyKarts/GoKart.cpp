// Fill out your copyright notice in the Description page of Project Settings.


#include "GoKart.h"
#include "Components/InputComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "UnrealNetwork.h"

// Sets default values
AGoKart::AGoKart()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
}

// Called when the game starts or when spawned
void AGoKart::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		NetUpdateFrequency = 1;
	}
}

void AGoKart::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AGoKart, ReplicatedTransform);
	DOREPLIFETIME(AGoKart, Velocity);
	DOREPLIFETIME(AGoKart, Throttle);
	DOREPLIFETIME(AGoKart, SteeringThrow);
}

FString GetEnumText(ENetRole Role)
{
	switch (Role)
	{
	case ENetRole::ROLE_Authority:
		return TEXT("Authority");
	case ENetRole::ROLE_SimulatedProxy:
		return TEXT("Simulated Proxy");
	case ENetRole::ROLE_AutonomousProxy:
		return TEXT("Autonomous Proxy");
	case ENetRole::ROLE_None:
		return TEXT("None");
	default:
		return TEXT("ERROR");
	}
}

// Called every frame
void AGoKart::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector Acceleration;
	FVector Force = MaxDrivingForce * Throttle * GetActorForwardVector();
	Force += GetAirResistance();
	Force += GetRollingResistance();

	Acceleration = Force / Mass;
	Velocity += Acceleration * DeltaTime;

	UpdateLocationFromVelocity(DeltaTime);

	ApplyRotation(DeltaTime);

	if (HasAuthority())
	{
		ReplicatedTransform = GetActorTransform();
	}

	DrawDebugString(GetWorld(), FVector(0.0f, 0.0f, 100.0f), GetEnumText(Role), this, FColor::White, DeltaTime);
}

//Only comes through on the client
void AGoKart::OnRep_ReplicatedTransform()
{
	UE_LOG(LogTemp, Warning, TEXT("Replicated Location"));
	SetActorTransform(ReplicatedTransform);
}

// Called to bind functionality to input
void AGoKart::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis("MoveForward", this, &AGoKart::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AGoKart::MoveRight);
}

void AGoKart::UpdateLocationFromVelocity(float DeltaTime)
{
	//100 converts from meters to centimeters.
	FVector Translation = 100.0f * Velocity * DeltaTime;
	FHitResult HitInfo;

	AddActorWorldOffset(Translation, true, &HitInfo);

	if (HitInfo.IsValidBlockingHit())
	{
		Velocity = FVector::ZeroVector;
	}
}

void AGoKart::ApplyRotation(float DeltaTime)
{
	float DeltaLocation = FVector::DotProduct(GetActorForwardVector(), Velocity) * DeltaTime;
	float RotationAngle = (DeltaLocation / MinTurningRadius) * SteeringThrow;
	FQuat RotationDelta(GetActorUpVector(), RotationAngle);
	Velocity = RotationDelta.RotateVector(Velocity);
	AddActorWorldRotation(RotationDelta);
}

FVector AGoKart::GetAirResistance()
{
	return -Velocity.GetSafeNormal() * Velocity.SizeSquared() * AirResistance;
}

FVector AGoKart::GetRollingResistance()
{
	float GravitationalAcceleration = -GetWorld()->GetGravityZ() / 100.0f;
	float NormalForce = Mass * GravitationalAcceleration;
	return -Velocity.GetSafeNormal() * RollingResistance * NormalForce;
}

void AGoKart::MoveForward(float Value)
{
	Throttle = Value;
	Server_MoveForward(Value);
}

void AGoKart::MoveRight(float Value)
{
	SteeringThrow = Value;
	Server_MoveRight(Value);
}

void AGoKart::Server_MoveForward_Implementation(float Value)
{
	Throttle = Value;
}

bool AGoKart::Server_MoveForward_Validate(float Value)
{
	return (FMath::Abs(Value) <= 1.0f);
}

void AGoKart::Server_MoveRight_Implementation(float Value)
{
	SteeringThrow = Value;
}

bool AGoKart::Server_MoveRight_Validate(float Value)
{
	return (FMath::Abs(Value) <= 1.0f);
}
