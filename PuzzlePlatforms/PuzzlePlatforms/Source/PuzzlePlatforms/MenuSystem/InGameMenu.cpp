// Fill out your copyright notice in the Description page of Project Settings.


#include "InGameMenu.h"
#include "Components/Button.h"

bool UInGameMenu::Initialize()
{
	if (!Super::Initialize()) return false;

	if (!ensure(CloseButton != nullptr)) return false;
	CloseButton->OnClicked.AddDynamic(this, &UInGameMenu::CloseButtonPressed);

	if (!ensure(MainMenuButton != nullptr)) return false;
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
}