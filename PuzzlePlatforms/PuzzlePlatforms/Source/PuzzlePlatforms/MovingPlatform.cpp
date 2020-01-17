// Fill out your copyright notice in the Description page of Project Settings.


#include "MovingPlatform.h"

AMovingPlatform::AMovingPlatform()
{
	PrimaryActorTick.bCanEverTick = true;
	SetMobility(EComponentMobility::Movable);
}

void AMovingPlatform::BeginPlay()
{
	Super::BeginPlay();

	//Only valid for servers to replicate objects.
	if (HasAuthority())
	{
		//Allows for this object to be replicated by the server.
		SetReplicates(true);

		//Replicates the movement of this object since it doesn't by default.
		SetReplicateMovement(true);
	}

	GlobalStartLocation = GetActorLocation();
	GlobalTargetLocation = GetTransform().TransformPosition(LocalTargetLocation);
}

void AMovingPlatform::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (ActiveTriggers > 0)
	{
		if (HasAuthority())
		{
			FVector Location = GetActorLocation();
			FVector Direction = (GlobalTargetLocation - GlobalStartLocation).GetSafeNormal();
			float CurrentLength = (Location - GlobalStartLocation).Size();
			float TotalLength = (GlobalTargetLocation - GlobalStartLocation).Size();

			if (CurrentLength >= TotalLength)
			{
				FVector Swap = GlobalTargetLocation;
				GlobalTargetLocation = GlobalStartLocation;
				GlobalStartLocation = Swap;
			}

			Location += Direction * Speed * DeltaSeconds;
			SetActorLocation(Location);
		}
	}
}

void AMovingPlatform::AddActiveTrigger()
{
	++ActiveTriggers;
}

void AMovingPlatform::RemoveActiveTrigger()
{
	if (ActiveTriggers > 0)
	{
		--ActiveTriggers;
	}
}