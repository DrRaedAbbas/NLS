// Fill out your copyright notice in the Description page of Project Settings.


#include "NLSGameInstance.h"

void UNLSGameInstance::Init()
{
	Super::Init();
	
	OnlineSubsystem = IOnlineSubsystem::Get();
	SetGameSettings();
}

void UNLSGameInstance::CreateNewSession()
{
	if (OnlineSession)
	{
		if (IOnlineSessionPtr SessionPtr = OnlineSubsystem->GetSessionInterface())
		{
			SessionPtr->OnCreateSessionCompleteDelegates.AddUObject(this, &UNLSGameInstance::OnCreateSessionComplete);
			SessionPtr->CreateSession(0, GameName, GameSettings);
		}
	}
}

void UNLSGameInstance::SetGameSettings()
{
	GameSettings.bIsDedicated = IsDedicated;
	GameSettings.bShouldAdvertise = true;
	GameSettings.bIsLANMatch = IsLAN;
	GameSettings.NumPublicConnections = MaxPlayers;
	GameSettings.bAllowJoinInProgress = true;
	GameSettings.bAllowJoinViaPresence = true;
	GameSettings.bUsesPresence = true;
}

void UNLSGameInstance::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	UE_LOG(LogTemp, Warning, TEXT("Game Session Created: %d, %s, Max Players: %i"), bWasSuccessful, *SessionName.ToString(), MaxPlayers);
	
	if (OnlineSession)
	{
		if (IOnlineSessionPtr SessionPtr = OnlineSubsystem->GetSessionInterface())
		{
			SessionPtr->ClearOnCreateSessionCompleteDelegates(this);
		}
	}
}