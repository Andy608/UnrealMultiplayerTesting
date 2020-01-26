// Fill out your copyright notice in the Description page of Project Settings.


#include "InGameMenu.h"
#include "Components/Button.h"

bool UInGameMenu::Initialize()
{
	if (!Super::Initialize())
	{
		UE_LOG(LogTemp, Warning, TEXT("[Initialize] SUPER INITIALIZE RETURNED FALSE."));
		return false;
	}

	if (!ensure(CloseButton != nullptr))
	{
		UE_LOG(LogTemp, Warning, TEXT("[Initialize] CLOSE BUTTON NULL."));
		return false;
	}
	CloseButton->OnClicked.AddDynamic(this, &UInGameMenu::CloseButtonPressed);

	if (!ensure(MainMenuButton != nullptr))
	{
		UE_LOG(LogTemp, Warning, TEXT("[Initialize] MAIN MENU BUTTON NULL."));
		return false;
	}
	MainMenuButton->OnClicked.AddDynamic(this, &UInGameMenu::MainMenuButtonPressed);

	return true;
}

void UInGameMenu::CloseButtonPressed()
{
	Close();
}

void UInGameMenu::MainMenuButtonPressed()
{
	if (IMenu != nullptr)
	{
		Close();
		IMenu->DisconnectAndLoadMenu();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[MainMenuButtonPressed] IMENU INTERFACE IS NULL."));
	}
}