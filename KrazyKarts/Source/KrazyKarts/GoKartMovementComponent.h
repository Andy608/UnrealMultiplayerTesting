// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GoKartMovementComponent.generated.h"

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

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class KRAZYKARTS_API UGoKartMovementComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UGoKartMovementComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void SimulateMove(const FGoKartMove& Move);

	inline const FVector& GetVelocity() const { return Velocity; }
	inline const FGoKartMove& GetLastMove() const { return LastMove; }

	void SetVelocity(const FVector& NewVelocity) { Velocity = NewVelocity; }

	void SetThrottle(float NewThrottle) { Throttle = NewThrottle; }
	void SetSteeringThrow(float NewSteeringThrow) { SteeringThrow = NewSteeringThrow; }

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

	//Newtons
	UPROPERTY(EditAnywhere)
	float MaxDrivingForce = 10000;

	FGoKartMove LastMove;

	FVector Velocity;
	float Throttle;
	float SteeringThrow;

	FVector GetAirResistance();
	FVector GetRollingResistance();

	void ApplyRotation(float DeltaTime, float SteeringThrow);
	void UpdateLocationFromVelocity(float DeltaTime);

	FGoKartMove CreateMove(float DeltaTime);
};
