// Fill out your copyright notice in the Description page of Project Settings.

#include "PuzzlePlatformsGameInstance.h"

#include "Engine/Engine.h"
#include "GameFramework/PlayerController.h"
#include "UObject/ConstructorHelpers.h"
#include "Blueprint/UserWidget.h"
#include "OnlineSessionSettings.h"

#include "PlatformTrigger.h"
#include "MainMenu.h"
#include "MenuSystem/MenuWidget.h"

const static FName SESSION_NAME = TEXT("My Session Game");

UPuzzlePlatformsGameInstance::UPuzzlePlatformsGameInstance(const FObjectInitializer& ObjectInitializer)
{
	UE_LOG(LogTemp, Warning, TEXT("[UPuzzlePlatformsGameInstance] GAME INSTANCE CONSTRUCTOR."));

	static ConstructorHelpers::FClassFinder<UUserWidget> MenuBPClass(TEXT("/Game/MenuSystem/WBP_MainMenu"));
	
	if (!ensure(MenuBPClass.Class != nullptr))
	{
		UE_LOG(LogTemp, Warning, TEXT("[UPuzzlePlatformsGameInstance] MENUBPCLASS.CLASS IS NULL."));
		return;
	}

	MenuClass = MenuBPClass.Class;

	static ConstructorHelpers::FClassFinder<UUserWidget> InGameMenuBPClass(TEXT("/Game/MenuSystem/WBP_InGameMenu"));

	if (!ensure(InGameMenuBPClass.Class != nullptr))
	{
		UE_LOG(LogTemp, Warning, TEXT("[UPuzzlePlatformsGameInstance] INGAMEMENUBPCLASS.CLASS IS NULL."));
		return;
	}

	InGameMenuClass = InGameMenuBPClass.Class;
}

void UPuzzlePlatformsGameInstance::Init()
{
	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();

	if (OnlineSubsystem != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Init] WE FOUND SUBSYSTEM: %s"), *OnlineSubsystem->GetSubsystemName().ToString());
		SessionInterface = OnlineSubsystem->GetSessionInterface();

		if (SessionInterface.IsValid())
		{
			SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UPuzzlePlatformsGameInstance::OnCreateSessionComplete);
			SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &UPuzzlePlatformsGameInstance::OnDestroySessionComplete);
			SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UPuzzlePlatformsGameInstance::OnFindSessionsComplete);
			SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UPuzzlePlatformsGameInstance::OnJoinSessionComplete);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[Init] SESSION INTERFACE IS NOT VALID."));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[Init] NO SUBSYSTEM FOUND."));
	}
}

void UPuzzlePlatformsGameInstance::LoadMainMenu()
{
	if (!ensure(MenuClass != nullptr))
	{
		UE_LOG(LogTemp, Warning, TEXT("[LoadMainMenu] IN GAME MENU CLASS IS NULL."));
		return;
	}

	Menu = CreateWidget<UMainMenu>(this, MenuClass);

	if (!ensure(Menu != nullptr))
	{
		UE_LOG(LogTemp, Warning, TEXT("[LoadMainMenu] MENU CLASS IS NULL."));
		return;
	}

	Menu->Setup();
	Menu->SetMenuInterface(this);
}

void UPuzzlePlatformsGameInstance::LoadInGameMenu()
{
	if (!ensure(InGameMenuClass != nullptr))
	{
		UE_LOG(LogTemp, Warning, TEXT("[LoadInGameMenu] IN GAME MENU CLASS IS NULL."));
		return;
	}

	UMenuWidget* Menu = CreateWidget<UMenuWidget>(this, InGameMenuClass);

	if (!ensure(Menu != nullptr))
	{
		UE_LOG(LogTemp, Warning, TEXT("[LoadInGameMenu] MENU IS NULL."));
		return;
	}

	Menu->Setup();
	Menu->SetMenuInterface(this);
}

void UPuzzlePlatformsGameInstance::Host()
{
	if (SessionInterface.IsValid())
	{
		FNamedOnlineSession* ExistingSession = SessionInterface->GetNamedSession(SESSION_NAME);

		if (ExistingSession != nullptr)
		{
			SessionInterface->DestroySession(SESSION_NAME);
		}
		else
		{
			CreateSession();
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[Host] SESSION INTERFACE IS NOT VALID."));
	}
}

void UPuzzlePlatformsGameInstance::OnDestroySessionComplete(FName SessionName, bool Success)
{
	if (Success)
	{
		CreateSession();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[OnDestroySessionComplete] SUCCESS RETURNED FALSE. CANNOT CREATE SESSION."));
	}
}

void UPuzzlePlatformsGameInstance::CreateSession()
{
	if (SessionInterface.IsValid())
	{
		FOnlineSessionSettings SessionSettings;
		SessionSettings.bIsLANMatch = false;
		SessionSettings.NumPublicConnections = 10;
		//If false, you'd have to send a personal invite for people to join.
		SessionSettings.bShouldAdvertise = true;
		SessionSettings.bUsesPresence = true;

		SessionInterface->CreateSession(0, SESSION_NAME, SessionSettings);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[CreateSession] SESSION INTERFACE IS NOT VALID."));
	}
}

void UPuzzlePlatformsGameInstance::OnCreateSessionComplete(FName SessionName, bool Success)
{
	if (!Success)
	{
		UE_LOG(LogTemp, Warning, TEXT("[OnCreateSessionComplete] SUCCESS RETURNED FALSE. CANNOT CREATE SESSION."));
		return;
	}

	if (Menu != nullptr)
	{
		Menu->Close();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[OnCreateSessionComplete] MENU IS NULL."));
	}

	UEngine* Engine = GetEngine();
	
	if (!ensure(Engine != nullptr))
	{
		UE_LOG(LogTemp, Warning, TEXT("[OnCreateSessionComplete] ENGINE IS NULL."));
		return;
	}

	Engine->AddOnScreenDebugMessage(0, 2.0f, FColor::Green, TEXT("Hosting!"));

	UWorld* World = GetWorld();
	
	if (!ensure(World != nullptr))
	{
		UE_LOG(LogTemp, Warning, TEXT("[OnCreateSessionComplete] WORLD IS NULL."));
		return;
	}

	World->ServerTravel("/Game/ThirdPersonCPP/Maps/ThirdPersonExampleMap?listen");
}

void UPuzzlePlatformsGameInstance::RefreshServerList()
{
	SessionSearch = MakeShareable(new FOnlineSessionSearch());

	if (SessionSearch.IsValid())
	{
		//Because we're sharing the default app id, we need to filter through all of the sessions we find.
		//If we didn't make MaxSearchResults a high number, we might find other people's games and not have room for
		//our own.
		//If we used a unique appid (by paying steam), then we wouldn't have to have this number so high,
		//but because we are sharing the same public appid with everyone else, it's necessary.
		SessionSearch->MaxSearchResults = 100;
		SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

		UE_LOG(LogTemp, Warning, TEXT("[RefreshServerList] ATTEMPTING TO FIND SESSIONS."));
		
		if (!SessionInterface->FindSessions(0, SessionSearch.ToSharedRef()))
		{
			UE_LOG(LogTemp, Warning, TEXT("[RefreshServerList] FINDING SESSIONS FAILED."));
		}
	}
}

void UPuzzlePlatformsGameInstance::OnFindSessionsComplete(bool Success)
{
	if (Success && SessionSearch.IsValid() && Menu != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("[OnFindSessionsComplete] FINISHED FINDING SESSIONS."));

		TArray<FString> ServerNames;
		for (const FOnlineSessionSearchResult& SearchResult : SessionSearch->SearchResults)
		{
			UE_LOG(LogTemp, Warning, TEXT("[OnFindSessionsComplete] FOUND SESSION NAMED: %s"), *SearchResult.GetSessionIdStr());
			ServerNames.Add(SearchResult.GetSessionIdStr());
		}

		Menu->SetServerList(ServerNames);
	}
}

void UPuzzlePlatformsGameInstance::Join(uint32 Index)
{
	if (!SessionInterface.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("[Join] SESSION INTERFACE IS NOT VALID."));
		return;
	}

	if (!SessionSearch.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("[Join] SESSION SEARH IS NOT VALID."));
		return;
	}

	if (Menu != nullptr)
	{
		Menu->Close();
	}

	UEngine* Engine = GetEngine();
	if (!ensure(Engine != nullptr))
	{
		UE_LOG(LogTemp, Warning, TEXT("[Join] ENGINE IS NULL."));
		return;
	}

	if (!SessionInterface->JoinSession(0, SESSION_NAME, SessionSearch->SearchResults[Index]))
	{
		Engine->AddOnScreenDebugMessage(0, 20.0f, FColor::Green, FString::Printf(TEXT("[Join] JOIN FAILED.")));
	}
}

void UPuzzlePlatformsGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if (!SessionInterface.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("[OnJoinSessionComplete] SESSION INTERFACE IS NOT VALID."));
		return;
	}

	FString Address;
	if (!SessionInterface->GetResolvedConnectString(SessionName, Address))
	{
		UE_LOG(LogTemp, Warning, TEXT("[OnJoinSessionComplete] COULD NOT GET CONNECTION STRING."));
		return;
	}

	UEngine* Engine = GetEngine();
	if (!ensure(Engine != nullptr))
	{
		UE_LOG(LogTemp, Warning, TEXT("[OnJoinSessionComplete] ENGINE IS NULL."));
		return;
	}
	
	Engine->AddOnScreenDebugMessage(0, 5.0f, FColor::Green, FString::Printf(TEXT("[OnJoinSessionComplete] Joining %s"), *Address));
	
	APlayerController* PlayerController = GetFirstLocalPlayerController();
	if (!ensure(PlayerController != nullptr))
	{
		UE_LOG(LogTemp, Warning, TEXT("[OnJoinSessionComplete] PLAYER CONTROLLER IS NULL."));
		return;
	}
	
	PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
}

void UPuzzlePlatformsGameInstance::DisconnectAndLoadMenu()
{
	APlayerController* PlayerController = GetFirstLocalPlayerController();
	if (!ensure(PlayerController != nullptr))
	{
		UE_LOG(LogTemp, Warning, TEXT("[DisconnectAndLoadMenu] PLAYER CONTROLLER IS NULL."));
		return;
	}

	PlayerController->ClientTravel("/Game/MenuSystem/MainMenu", ETravelType::TRAVEL_Absolute);
}