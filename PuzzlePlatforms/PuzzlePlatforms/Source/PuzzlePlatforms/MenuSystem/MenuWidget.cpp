// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuWidget.h"

UMenuWidget::UMenuWidget(const FObjectInitializer& ObjectInitializer) :
	UUserWidget(ObjectInitializer)
{
}

void UMenuWidget::Setup()
{
	AddToViewport();

	UWorld* World = GetWorld();
	if (!ensure(World != nullptr))
	{
		UE_LOG(LogTemp, Warning, TEXT("[Setup] WORLD IS NULL."));
		return;
	}

	APlayerController* PlayerController = World->GetFirstPlayerController();
	if (!ensure(PlayerController != nullptr))
	{
		UE_LOG(LogTemp, Warning, TEXT("[Setup] PLAYER CONTROLLER IS NULL."));
		return;
	}

	FInputModeUIOnly InputModeData;
	InputModeData.SetWidgetToFocus(TakeWidget());
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

	PlayerController->SetInputMode(InputModeData);

	PlayerController->bShowMouseCursor = true;
}

void UMenuWidget::Close()
{
	UWorld* World = GetWorld();
	if (!ensure(World != nullptr))
	{
		UE_LOG(LogTemp, Warning, TEXT("[Close] WORLD IS NULL."));
		return;
	}

	APlayerController* PlayerController = World->GetFirstPlayerController();
	if (!ensure(PlayerController != nullptr))
	{
		UE_LOG(LogTemp, Warning, TEXT("[Close] PLAYER CONTROLLER IS NULL."));
		return;
	}

	Close(PlayerController->GetLevel(), World);
}

void UMenuWidget::Close(ULevel* InLevel, UWorld* InWorld)
{
	RemoveFromViewport();

	UWorld* World = InWorld;
	if (!ensure(World != nullptr))
	{
		UE_LOG(LogTemp, Warning, TEXT("[Close(ULevel*, World*)] WORLD IS NULL."));
		return;
	}

	APlayerController* PlayerController = World->GetFirstPlayerController();
	if (!ensure(PlayerController != nullptr))
	{
		UE_LOG(LogTemp, Warning, TEXT("[Close(ULevel*, World*)] PLAYER CONTROLLER IS NULL."));
		return;
	}

	FInputModeGameOnly InputModeData;
	PlayerController->SetInputMode(InputModeData);
	PlayerController->bShowMouseCursor = false;
}

void UMenuWidget::SetMenuInterface(IMenuInterface* MenuInterface)
{
	IMenu = MenuInterface;
}

void UMenuWidget::OnLevelRemovedFromWorld(ULevel* InLevel, UWorld* InWorld)
{
	Close(InLevel, InWorld);
}