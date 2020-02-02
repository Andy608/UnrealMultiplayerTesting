// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GoKart.generated.h"

USTRUCT()
struct FGoKartMove
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	float TimeStamp;

	UPROPERTY()
	float Throttle;

	UPROPERTY()
	float SteeringThrow;

	UPROPERTY()
	float DeltaTime;
};

USTRUCT()
struct FGoKartState
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	FGoKartMove LastMove;

	UPROPERTY()
	FVector Velocity;

	UPROPERTY()
	FTransform Transform;
};

UCLASS()
class KRAZYKARTS_API AGoKart : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AGoKart();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	//Kilograms
	UPROPERTY(EditAnywhere)
	float Mass = 1000;

	//Degrees per second
	UPROPERTY(EditAnywhere)
	float MaxDegreesPerSecond = 90;

	//Higher means more air resistance
	UPROPERTY(EditAnywhere)
	float AirResistance = 16;

	//Higher means more rolling resistance
	UPROPERTY(EditAnywhere)
	float RollingResistance = 0.05;

	//Minimum radius for our car to turn in meters
	UPROPERTY(EditAnywhere)
	float MinTurningRadius = 10;

	UPROPERTY(ReplicatedUsing = OnRep_ServerState)
	FGoKartState ServerState;

	FVector Velocity;
	float Throttle;
	float SteeringThrow;

	TArray<FGoKartMove> UnacknowledgedMoves;

	UFUNCTION()
	void OnRep_ServerState();

	//Newtons
	UPROPERTY(EditAnywhere)
	float MaxDrivingForce = 10000;

	void UpdateLocationFromVelocity(float DeltaTime);
	void ApplyRotation(float DeltaTime, float SteeringThrow);

	FVector GetAirResistance();
	FVector GetRollingResistance();

	void MoveForward(float Value);
	void MoveRight(float Value);

	void SimulateMove(const FGoKartMove& Move);

	FGoKartMove CreateMove(float DeltaTime);
	void ClearAcknowledgedMoves(FGoKartMove LastMove);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SendMove(FGoKartMove Move);
};
