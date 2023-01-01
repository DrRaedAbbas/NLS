// Copy righted to RAED ABBAS 2023


#include "MGS_OnlineSubsystem.h"
#include "MGSFunctionLibrary.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystemUtils.h"

#include "OnlineSubsystem.h"

UMGS_OnlineSubsystem::UMGS_OnlineSubsystem():
OnLoginCompleteDelegate(FOnLoginCompleteDelegate::CreateUObject(this, &ThisClass::OnLoginWithEOSCompleted)),
OnChatLoginCompleteDelegate(FOnVoiceChatLoginCompleteDelegate::CreateUObject(this, &ThisClass::OnLoginWithEOSvChatCompleted)),
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

void UMGS_OnlineSubsystem::SetGameSettings(int32 MaxPlayers, FString MatchType/*, FString LevelPath*/, bool bIsDedicatedServer)
{
	SessionSettings = MakeShareable(new FOnlineSessionSettings());
	SessionSettings->NumPublicConnections = MaxPlayers;
	SessionSettings->bIsLANMatch = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL" ? true : false;
	SessionSettings->bIsDedicated = bIsDedicatedServer;
	SessionSettings->bAllowJoinInProgress = true;
	SessionSettings->bAllowJoinViaPresence = true;
	SessionSettings->bShouldAdvertise = true;
	SessionSettings->bUsesPresence = true;
	SessionSettings->bUseLobbiesIfAvailable = false;
	SessionSettings->bUseLobbiesVoiceChatIfAvailable = false;
	SessionSettings->bAllowInvites = true;
	SessionSettings->BuildUniqueId = 1;
	SessionSettings->Set(FName("MatchType"), MatchType, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
}

FString UMGS_OnlineSubsystem::GetCurrentPlayerName()
{
	IOnlineSubsystem* SubsystemRef = Online::GetSubsystem(this->GetWorld());
	if (SubsystemRef)
	{
		IdentityPtr = SubsystemRef->GetIdentityInterface();
		if (!IdentityPtr.IsValid())
		{
			MGSFunctionLibrary->DisplayDebugMessage(FString(TEXT("Identity Pointer Not valid!")), FColor::Red);
			return FString();
		}
		if (IdentityPtr->GetLoginStatus(0) == ELoginStatus::LoggedIn) return IdentityPtr->GetPlayerNickname(0);
	}
	return FString();
}

void UMGS_OnlineSubsystem::GetPlayerStatus(bool& IsLoggedIn, FString& OutStatus)
{
	IOnlineSubsystem* SubsystemRef = Online::GetSubsystem(this->GetWorld());
	if (SubsystemRef)
	{
		IdentityPtr = SubsystemRef->GetIdentityInterface();
		if (!IdentityPtr.IsValid())
		{
			MGSFunctionLibrary->DisplayDebugMessage(FString(TEXT("Identity Pointer Not valid!")), FColor::Red);
			return;
		}
		OutStatus = ToString(IdentityPtr->GetLoginStatus(0));
		IsLoggedIn = IdentityPtr->GetLoginStatus(0) == ELoginStatus::LoggedIn;
	}
}

bool UMGS_OnlineSubsystem::IsPlayerLoggedIn()
{
	IOnlineSubsystem* SubsystemRef = Online::GetSubsystem(this->GetWorld());
	if (SubsystemRef)
	{
		IdentityPtr = SubsystemRef->GetIdentityInterface();
		if (!IdentityPtr.IsValid())
		{
			MGSFunctionLibrary->DisplayDebugMessage(FString(TEXT("Identity Pointer Not valid!")), FColor::Red);
			return false;
		}
		return IdentityPtr->GetLoginStatus(0) == ELoginStatus::LoggedIn;
	}
	MGSFunctionLibrary->DisplayDebugMessage(FString(TEXT("Subsystem Pointer Not valid!")), FColor::Red);
	return false;
}

//***********************LOGIN*******************
void UMGS_OnlineSubsystem::LoginWithEOS(FString ID, FString Token, FString LoginType)
{
	EOSLoginCompleted.AddDynamic(this, &ThisClass::LoginEOSCompleted);
	IOnlineSubsystem* SubsystemRef = Online::GetSubsystem(this->GetWorld());
	if (SubsystemRef)
	{
		IdentityPtr = SubsystemRef->GetIdentityInterface();
		if(!IdentityPtr.IsValid())
		{
			MGSFunctionLibrary->DisplayDebugMessage(FString(TEXT("Identity Pointer Not valid!")), FColor::Red);
			EOSLoginCompleted.Broadcast(false);
			return;
		}
		LoginHandle = IdentityPtr->AddOnLoginCompleteDelegate_Handle(0, OnLoginCompleteDelegate);
		FOnlineAccountCredentials Credentials;
		Credentials.Id = "";
		Credentials.Token = "";
		Credentials.Type = "accountportal";
		
		//IdentityPtr->OnLoginCompleteDelegates->AddUObject(this, &ThisClass::OnLoginWithEOSCompleted);
		if (!IdentityPtr->Login(0, Credentials))
		{
			IdentityPtr->ClearOnLoginCompleteDelegate_Handle(0, LoginHandle);
			MGSFunctionLibrary->DisplayDebugMessage(FString(TEXT("Login Failed!")), FColor::Red);
		}
		;
	}
}
void UMGS_OnlineSubsystem::OnLoginWithEOSCompleted(int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserID,
                                                   const FString& Error)
{
	if(IdentityPtr) IdentityPtr->ClearOnLoginCompleteDelegate_Handle(0, LoginHandle);
	if (bWasSuccessful)
	{
		MGSFunctionLibrary->DisplayDebugMessage(FString(TEXT("Login Success")), FColor::Green);
	}
	else
	{
		MGSFunctionLibrary->DisplayDebugMessage(FString(Error), FColor::Red);
	}
	EOSLoginCompleted.Broadcast(bWasSuccessful);
}
void UMGS_OnlineSubsystem::LoginEOSCompleted(bool bSuccess)
{
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

	MGSFunctionLibrary->DisplayDebugMessage(FString(TEXT("Creating Game Session")), FColor::Blue);

	CreateSessionHandle = SessionInterface->AddOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegate);
	/*SessionSettings = MakeShareable(new FOnlineSessionSettings());
	SessionSettings->bIsLANMatch = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL" ? true : false;
	SessionSettings->NumPublicConnections = MaxPlayers;
	SessionSettings->bAllowJoinInProgress = true;
	SessionSettings->bAllowJoinViaPresence = true;
	SessionSettings->bShouldAdvertise = true;
	SessionSettings->bUsesPresence = true;
	SessionSettings->Set(FName("MatchType"), MatchType, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	SessionSettings->BuildUniqueId = 1;
	SessionSettings->bUseLobbiesIfAvailable = true;
	SessionSettings->bUseLobbiesVoiceChatIfAvailable = true;*/
	
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
	
	TArray<FOnlineSessionSearchResult> SearchResults = SessionSearch->SearchResults;
	for (FOnlineSessionSearchResult SearchResult : SearchResults)
	{
		FBlueprintSessionResult BPResult;
		BPResult.OnlineResult = SearchResult;
		SessionSearchResults.Add(BPResult);
	}
	
	MGSFindSessionsCompleted.Broadcast(SearchResults, bWasSuccessful);

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

//*******************Voice Chat***************************
void UMGS_OnlineSubsystem::LoginWithEOSvChat()
{
	EOSvChatLoginCompleted.AddDynamic(this, &ThisClass::LoginEOSvChatCompleted);
	IOnlineSubsystem* SubsystemRef = Online::GetSubsystem(this->GetWorld());
	if (SubsystemRef)
	{
		IdentityPtr = SubsystemRef->GetIdentityInterface();
		if (!IdentityPtr.IsValid())
		{
			MGSFunctionLibrary->DisplayDebugMessage(FString(TEXT("Identity Pointer Not valid!")), FColor::Red);
			EOSvChatLoginCompleted.Broadcast(false);
			return;
		}
		if (!IsPlayerLoggedIn())
		{
			EOSvChatLoginCompleted.Broadcast(false);
			return;
		}
		IVoiceChat* vChatRef = IVoiceChat::Get();
		if(vChatRef) vChatUser = vChatRef->CreateUser();
		if (vChatUser == nullptr)
		{
			EOSvChatLoginCompleted.Broadcast(false);
			return;
		}
		/*TSharedPtr<const FUniqueNetId>*/ vChatUserID = IdentityPtr->GetUniquePlayerId(0);
		FPlatformUserId PlatformUserID = IdentityPtr->GetPlatformUserIdFromUniqueNetId(*vChatUserID);
		vChatUser->Login(PlatformUserID, vChatUserID->ToString(), TEXT(""), OnChatLoginCompleteDelegate);
	}
}
void UMGS_OnlineSubsystem::OnLoginWithEOSvChatCompleted(const FString& PlayerName, const FVoiceChatResult& Result)
{
	if (Result.IsSuccess())
	{
		MGSFunctionLibrary->DisplayDebugMessage(FString(TEXT("vChat Login Success")), FColor::Green);
	}
	EOSvChatLoginCompleted.Broadcast(Result.IsSuccess());
}
void UMGS_OnlineSubsystem::LoginEOSvChatCompleted(bool bSuccess)
{
}

void UMGS_OnlineSubsystem::SetEOSvChatVoiceVolmue(float NewVolume)
{
	if (vChatUser)
	{
		vChatUser->SetPlayerVolume(vChatUserID->ToString(), NewVolume);
	}
}

void UMGS_OnlineSubsystem::SetEOSvChatVoiceMute(bool bMute)
{
	if (vChatUser)
	{
		vChatUser->SetPlayerVolume(vChatUserID->ToString(), bMute);
	}
}

void UMGS_OnlineSubsystem::GetInputOutput(TArray<FString>& InputDeviceNames, TArray<FString>& OutputDeviceNames)
{
	if (vChatUser)
	{
		for (auto InputDevice : vChatUser->GetAvailableInputDeviceInfos())
		{
			InputDeviceNames.Add(InputDevice.DisplayName);
		}
		for (auto OutputDevice : vChatUser->GetAvailableOutputDeviceInfos())
		{
			OutputDeviceNames.Add(OutputDevice.DisplayName);
		}
	}
}

void UMGS_OnlineSubsystem::SetInputOutputs(FString InputDeviceName, FString OutputDeviceName)
{
	if (vChatUser)
	{
		vChatUser->SetInputDeviceId(InputDeviceName);
		vChatUser->SetInputDeviceId(OutputDeviceName);
	}
}
