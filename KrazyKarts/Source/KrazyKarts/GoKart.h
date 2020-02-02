// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GoKartMovementComponent.h"
#include "GoKartReplicationComponent.h"
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

	//meta = (UPROPERTY(VisibleAnywhere, BlueprintReadOnly, AllowPrivateAccess = "true") - can do this instead of making this field public
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UGoKartReplicationComponent* ReplicationComponent;

private:
	UPROPERTY(VisibleAnywhere)
	UGoKartMovementComponent* MovementComponent;

	void MoveForward(float Value);
	void MoveRight(float Value);
};
