// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenu.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"
#include "MenuSystem/ServerRow.h"

UMainMenu::UMainMenu(const FObjectInitializer& ObjectInitializer) :
	UMenuWidget(ObjectInitializer)
{
	static ConstructorHelpers::FClassFinder<UUserWidget> ServerRowBPClass(TEXT("/Game/MenuSystem/WBP_ServerRow"));

	if (!ensure(ServerRowBPClass.Class != nullptr))
	{
		UE_LOG(LogTemp, Warning, TEXT("[UMainMenu] SERVERROWBPCLASS.CLASS IS NULL"));
		return;
	}

	ServerRowClass = ServerRowBPClass.Class;
}

bool UMainMenu::Initialize()
{
	if (!Super::Initialize())
	{
		UE_LOG(LogTemp, Warning, TEXT("[Initialize] SUPER INITIALIZE RETURNED FALSE."));
		return false;
	}

	if (!ensure(HostButton != nullptr))
	{
		UE_LOG(LogTemp, Warning, TEXT("[Initialize] HOST BUTTON IS NULL."));
		return false;
	}
	HostButton->OnClicked.AddDynamic(this, &UMainMenu::HostServer);

	if (!ensure(JoinButton != nullptr))
	{
		UE_LOG(LogTemp, Warning, TEXT("[Initialize] JOIN BUTTON IS NULL."));
		return false;
	}
	JoinButton->OnClicked.AddDynamic(this, &UMainMenu::OpenJoinMenu);

	if (!ensure(QuitButton != nullptr))
	{
		UE_LOG(LogTemp, Warning, TEXT("[Initialize] QUIT BUTTON IS NULL."));
		return false;
	}
	QuitButton->OnClicked.AddDynamic(this, &UMainMenu::QuitPressed);

	if (!ensure(IPJoinBackButton != nullptr))
	{
		UE_LOG(LogTemp, Warning, TEXT("[Initialize] IP JOIN BACK BUTTON IS NULL."));
		return false;
	}
	IPJoinBackButton->OnClicked.AddDynamic(this, &UMainMenu::OpenMainMenu);

	if (!ensure(IPJoinButton != nullptr))
	{
		UE_LOG(LogTemp, Warning, TEXT("[Initialize] IP JOIN BUTTON IS NULL."));
		return false;
	}
	IPJoinButton->OnClicked.AddDynamic(this, &UMainMenu::JoinServer);

	return true;
}

void UMainMenu::HostServer()
{
	if (IMenu != nullptr)
	{
		IMenu->Host();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[HostServer] IMENU INTERFACE IS NULL."));
	}
}

void UMainMenu::SetServerList(TArray<FString> ServerNames)
{
	UWorld* World = GetWorld();
	if (!ensure(World != nullptr))
	{
		UE_LOG(LogTemp, Warning, TEXT("[SetServerList] WORLD IS NULL."));
		return;
	}

	ServerList->ClearChildren();

	uint32 i = 0;
	for (const FString& ServerName : ServerNames)
	{
		UServerRow* Row = CreateWidget<UServerRow>(World, ServerRowClass);
		if (!ensure(Row != nullptr))
		{
			UE_LOG(LogTemp, Warning, TEXT("[SetServerList] ROW IS NULL."));
			return;
		}

		Row->ServerName->SetText(FText::FromString(ServerName));
		Row->Setup(this, i);
		++i;

		ServerList->AddChild(Row);
	}
}

void UMainMenu::SelectIndex(uint32 Index)
{
	SelectedIndex = Index;
}

void UMainMenu::JoinServer()
{
	if (SelectedIndex.IsSet() && IMenu != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("[JoinServer] SELECTED INDEX: %d."), SelectedIndex.GetValue());
		IMenu->Join(SelectedIndex.GetValue());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[JoinServer] SELECTED INDEX NOT SET."));
	}
}

void UMainMenu::OpenJoinMenu()
{
	if (!ensure(MenuSwitcher != nullptr))
	{
		UE_LOG(LogTemp, Warning, TEXT("[OpenJoinMenu] MENU SWITCH IS NULL."));
		return;
	}

	if (!ensure(JoinMenu != nullptr))
	{
		UE_LOG(LogTemp, Warning, TEXT("[OpenJoinMenu] JOIN MENU IS NULL."));
		return;
	}

	MenuSwitcher->SetActiveWidget(JoinMenu);

	if (IMenu != nullptr)
	{
		IMenu->RefreshServerList();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[OpenJoinMenu] IMENU INTERFACE IS NULL."));
	}
}

void UMainMenu::OpenMainMenu()
{
	if (!ensure(MenuSwitcher != nullptr))
	{
		UE_LOG(LogTemp, Warning, TEXT("[OpenMainMenu] MENU SWITCH IS NULL."));
		return;
	}

	if (!ensure(MainMenu != nullptr))
	{
		UE_LOG(LogTemp, Warning, TEXT("[OpenMainMenu] MAIN MENU IS NULL."));
		return;
	}

	MenuSwitcher->SetActiveWidget(MainMenu);
}

void UMainMenu::QuitPressed()
{
	UWorld* World = GetWorld();
	if (!ensure(World != nullptr))
	{
		UE_LOG(LogTemp, Warning, TEXT("[QuitPressed] WORLD IS NULL."));
		return;
	}

	APlayerController* PlayerController = World->GetFirstPlayerController();
	if (!ensure(PlayerController != nullptr))
	{
		UE_LOG(LogTemp, Warning, TEXT("[QuitPressed] PLAYER CONTROLLER IS NULL."));
		return;
	}

	PlayerController->ConsoleCommand("quit");
}
