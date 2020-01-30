// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"
#include "TimerManager.h"
#include "PuzzlePlatformsGameInstance.h"

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	++PlayerCount;

	if (PlayerCount == 4)
	{
		GetWorldTimerManager().SetTimer(GameStartTimer, this, &ALobbyGameMode::StartGame, 5.0f);
		UE_LOG(LogTemp, Warning, TEXT("[PostLogin] READY TO START THE MATCH!"));
	}
}

void ALobbyGameMode::StartGame()
{
	UPuzzlePlatformsGameInstance* GameInstance = Cast<UPuzzlePlatformsGameInstance>(GetGameInstance());

	if (GameInstance == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("[StartGame] GAME INSTANCE IS NULL."));
	}
	else
	{
		GameInstance->StartSession();
	}

	UWorld* World = GetWorld();

	if (!ensure(World != nullptr))
	{
		UE_LOG(LogTemp, Warning, TEXT("[StartGame] WORLD IS NULL."));
		return;
	}

	bUseSeamlessTravel = true;
	World->ServerTravel("/Game/PuzzlePlatforms/Maps/Game?listen");
}

void ALobbyGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	--PlayerCount;
}