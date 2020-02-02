// Fill out your copyright notice in the Description page of Project Settings.


#include "GoKart.h"
#include "Components/InputComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"

// Sets default values
AGoKart::AGoKart()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	bReplicateMovement = false;

	MovementComponent = CreateDefaultSubobject<UGoKartMovementComponent>(TEXT("MovementComponent"));
	ReplicationComponent = CreateDefaultSubobject<UGoKartReplicationComponent>(TEXT("ReplicationComponent"));
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

	DrawDebugString(GetWorld(), FVector(0.0f, 0.0f, 100.0f), GetEnumText(Role), this, FColor::White, DeltaTime);
}

// Called to bind functionality to input
void AGoKart::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis("MoveForward", this, &AGoKart::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AGoKart::MoveRight);
}

void AGoKart::MoveForward(float Value)
{
	if (MovementComponent == nullptr)
	{
		return;
	}

	MovementComponent->SetThrottle(Value);
}

void AGoKart::MoveRight(float Value)
{
	if (MovementComponent == nullptr)
	{
		return;
	}

	MovementComponent->SetSteeringThrow(Value);
}
