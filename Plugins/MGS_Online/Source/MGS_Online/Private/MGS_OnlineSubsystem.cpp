// Copy righted to RAED ABBAS 2023


#include "MGS_OnlineSubsystem.h"
#include "MGSFunctionLibrary.h"
#include "OnlineSessionSettings.h"

#include "OnlineSubsystem.h"

UMGS_OnlineSubsystem::UMGS_OnlineSubsystem():
CreateSessionCompleteDelegate(FOnCreateSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnCreateSessionCompleted)),
FindSessionsCompleteDelegate(FOnFindSessionsCompleteDelegate::CreateUObject(this, &ThisClass::OnFindSessionsCompleted)),
JoinSessionCompleteDelegate(FOnJoinSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnJoinSessionCompleted)),
StartSessionCompleteDelegate(FOnStartSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnStartSessionCompleted)),
DestroySessionCompleteDelegate(FOnDestroySessionCompleteDelegate::CreateUObject(this, &ThisClass::OnDestroySessionCompleted))
{
	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
	if(Subsystem)
	{
		SessionInterface = Subsystem->GetSessionInterface();
	}
}

//*******************Creating Session**********************
void UMGS_OnlineSubsystem::CreateGameSession(int32 MaxPlayers, FString MatchType)
{
	if (!SessionInterface) return;

	if (auto ExistingSession = SessionInterface->GetNamedSession(NAME_GameSession))
	{
		bCreateSessionOnDestroy = true;
		LastMaxPlayers = MaxPlayers;
		LastMatchType = MatchType;

		DestroyGameSession();
	}
		//SessionInterface->DestroySession(NAME_GameSession);

	MGSFunctionLibrary->DisplayDebugMessage(FString(TEXT("Creating Game Session")), FColor::Blue);

	CreateSessionHandle = SessionInterface->AddOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegate);
	SessionSettings = MakeShareable(new FOnlineSessionSettings());
	SessionSettings->bIsLANMatch = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL" ? true : false;
	SessionSettings->NumPublicConnections = MaxPlayers;
	SessionSettings->bAllowJoinInProgress = true;
	SessionSettings->bAllowJoinViaPresence = true;
	SessionSettings->bShouldAdvertise = true;
	SessionSettings->bUsesPresence = true;
	SessionSettings->Set(FName("MatchType"), MatchType, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	SessionSettings->BuildUniqueId = 1;
	SessionSettings->bUseLobbiesIfAvailable = true;
	SessionSettings->bUseLobbiesVoiceChatIfAvailable = true;

	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	const FUniqueNetId& LocalPlayerID = *LocalPlayer->GetPreferredUniqueNetId();

	if (!SessionInterface->CreateSession(LocalPlayerID, NAME_GameSession, *SessionSettings))
	{
		SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionHandle);
		MGSCreateSessionCompleted.Broadcast(false);
	}
}
void UMGS_OnlineSubsystem::OnCreateSessionCompleted(FName SessionName, bool bWasSuccessful)
{
	if (SessionInterface) SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionHandle);
	MGSCreateSessionCompleted.Broadcast(bWasSuccessful);

	MGSFunctionLibrary->DisplayDebugMessage(FString(TEXT("Create Session Completed")), FColor::Green);
}

//*******************Finding Session***********************
void UMGS_OnlineSubsystem::FindGameSessions(int32 MaxSearchResults)
{
	if (!SessionInterface.IsValid()) return;

	MGSFunctionLibrary->DisplayDebugMessage(FString(TEXT("Finding Game Sessions")), FColor::Blue);

	FindSessionsHandle = SessionInterface->AddOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegate);
	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	SessionSearch->MaxSearchResults = MaxSearchResults;
	SessionSearch->bIsLanQuery = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL" ? true : false;
	SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);
	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	if(!SessionInterface->FindSessions(*LocalPlayer->GetPreferredUniqueNetId(), SessionSearch.ToSharedRef()))
	{
		SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(FindSessionsHandle);
		MGSFindSessionsCompleted.Broadcast(TArray<FOnlineSessionSearchResult>(), false);
	}
}
void UMGS_OnlineSubsystem::OnFindSessionsCompleted(bool bWasSuccessful)
{
	if (SessionInterface) SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(FindSessionsHandle);
	if (SessionSearch->SearchResults.Num() <= 0)
	{
		MGSFindSessionsCompleted.Broadcast(TArray<FOnlineSessionSearchResult>(), false);
		MGSFunctionLibrary->DisplayDebugMessage(FString(TEXT("No Session Found!")), FColor::Red);
		return;
	}
	MGSFindSessionsCompleted.Broadcast(SessionSearch->SearchResults, bWasSuccessful);

	MGSFunctionLibrary->DisplayDebugMessage(FString(TEXT("Find Session Completed")), FColor::Green);
}

//*******************Joining Session***********************
void UMGS_OnlineSubsystem::JoinGameSession(const FOnlineSessionSearchResult& SessionSearchResult)
{
	if (!SessionInterface.IsValid())
	{
		MGSJoinSessionCompleted.Broadcast(EOnJoinSessionCompleteResult::UnknownError);
		return;
	}

	MGSFunctionLibrary->DisplayDebugMessage(FString(TEXT("Joining Game Session")), FColor::Blue);

	JoinSessionHandle = SessionInterface->AddOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegate);
	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	if (!SessionInterface->JoinSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, SessionSearchResult))
	{
		SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(JoinSessionHandle);
		MGSJoinSessionCompleted.Broadcast(EOnJoinSessionCompleteResult::UnknownError);
	}
}
void UMGS_OnlineSubsystem::OnJoinSessionCompleted(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if (SessionInterface) SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(JoinSessionHandle);
	MGSJoinSessionCompleted.Broadcast(Result);

	MGSFunctionLibrary->DisplayDebugMessage(FString(TEXT("Join Session Completed")), FColor::Green);
}

//*******************Starting Game*************************
void UMGS_OnlineSubsystem::StartGameSession()
{
}
void UMGS_OnlineSubsystem::OnStartSessionCompleted(FName SessionName, bool bWasSuccessful)
{
}

//*******************Destroying Session********************
void UMGS_OnlineSubsystem::DestroyGameSession()
{
	if (!SessionInterface.IsValid())
	{
		MGSDestroySessionCompleted.Broadcast(false);
		return;
	}
	MGSFunctionLibrary->DisplayDebugMessage(FString(TEXT("Destroying Game Session")), FColor::Blue);
	DestroySessionHandle = SessionInterface->AddOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegate);
	if (!SessionInterface->DestroySession(NAME_GameSession))
	{
		SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(DestroySessionHandle);
		MGSDestroySessionCompleted.Broadcast(false);
	}
}
void UMGS_OnlineSubsystem::OnDestroySessionCompleted(FName SessionName, bool bWasSuccessful)
{
	if (SessionInterface)
	{
		SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(DestroySessionHandle);
	}
	if (bWasSuccessful && bCreateSessionOnDestroy)
	{
		MGSFunctionLibrary->DisplayDebugMessage(FString(TEXT("Destroy Session Completed")), FColor::Green);
		bCreateSessionOnDestroy = false;
		CreateGameSession(LastMaxPlayers, LastMatchType);
	}
	MGSDestroySessionCompleted.Broadcast(bWasSuccessful);
}
