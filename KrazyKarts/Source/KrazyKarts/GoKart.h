// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GoKart.generated.h"

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

	UPROPERTY(Replicated)
	FVector Velocity;
	
	UPROPERTY(ReplicatedUsing = OnRep_ReplicatedTransform)
	FTransform ReplicatedTransform;

	UFUNCTION()
	void OnRep_ReplicatedTransform();

	UPROPERTY(Replicated)
	float Throttle;

	UPROPERTY(Replicated)
	float SteeringThrow;

	//Newtons
	UPROPERTY(EditAnywhere)
	float MaxDrivingForce = 10000;

	void UpdateLocationFromVelocity(float DeltaTime);
	void ApplyRotation(float DeltaTime);

	FVector GetAirResistance();
	FVector GetRollingResistance();

	void MoveForward(float Value);
	void MoveRight(float Value);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_MoveForward(float Value);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_MoveRight(float Value);
};