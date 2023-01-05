// Copy righted to RAED ABBAS 2023


#include "MGS_OnlineSubsystem.h"
#include "MGSFunctionLibrary.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystemUtils.h"
#include "OnlineSubsystem.h"
#include "Kismet/GameplayStatics.h"

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

void UMGS_OnlineSubsystem::SetGameSettings(int32 MaxPlayers, FString MatchType, bool bIsDedicatedServer)
{
	/*FName NewSettings = FName();
	SessionSettings->Set(NewSettings, MatchType, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);*/

	SessionSettings = MakeShareable(new FOnlineSessionSettings());
	SessionSettings->NumPublicConnections = MaxPlayers;
	SessionSettings->bIsLANMatch = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL" ? true : false;
	SessionSettings->bIsDedicated = bIsDedicatedServer;
	SessionSettings->bAllowJoinInProgress = true;
	SessionSettings->bAllowJoinViaPresence = true;
	SessionSettings->bShouldAdvertise = true;
	SessionSettings->bUsesPresence = true;
	SessionSettings->bUseLobbiesIfAvailable = IsPlayerLoggedIn() ? true : false;
	SessionSettings->bUseLobbiesVoiceChatIfAvailable = IsPlayerLoggedIn() ? true : false;
	SessionSettings->bAllowInvites = true;
	SessionSettings->BuildUniqueId = 1;
	SessionSettings->Set(FName("MatchType"), MatchType, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
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
	SetGameSettings(MaxPlayers, MatchType, SessionSettings->bIsDedicated);
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
	SessionSearch->QuerySettings.Set(SEARCH_LOBBIES, true, EOnlineComparisonOp::Equals);
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

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
//*******************EOS**************************
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

//*******************EOS LOGIN*******************
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

void UMGS_OnlineSubsystem::LoginWithEOS(FString ID, FString Token, FString LoginType)
{
	if (IsPlayerLoggedIn()) return;

	EOSLoginCompleted.AddDynamic(this, &ThisClass::LoginEOSCompleted);
	IOnlineSubsystem* SubsystemRef = Online::GetSubsystem(this->GetWorld());
	if (SubsystemRef)
	{
		IdentityPtr = SubsystemRef->GetIdentityInterface();
		if (!IdentityPtr.IsValid())
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

void UMGS_OnlineSubsystem::OnLoginWithEOSCompleted(int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserID, const FString& Error)
{
	if (IdentityPtr) IdentityPtr->ClearOnLoginCompleteDelegate_Handle(0, LoginHandle);
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
	if (!bSuccess)
	{
		UE_LOG(LogTemp, Error, TEXT("EOS Login completed but FAILED!!!!!!"));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("EOS Login completed Success :) "));

	IOnlineSubsystem* SubsystemEOS = Online::GetSubsystem(this->GetWorld());
	if (SubsystemEOS)
	{
		IVoiceChat* vChatRef = IVoiceChat::Get();
		if (vChatRef) vChatUser = vChatRef->CreateUser();
	}

	if (vChatUser)
	{
		FString vChatUserName = vChatUser->GetLoggedInPlayerName();
		UE_LOG(LogTemp, Warning, TEXT("vChat User: &s"), *vChatUserName);

		/*vChatUserID = IdentityPtr->GetUniquePlayerId(0);
		FPlatformUserId PlatformUserID = IdentityPtr->GetPlatformUserIdFromUniqueNetId(*vChatUserID);
		vChatUser->Login(PlatformUserID, vChatUserID->ToString(), TEXT(""), OnChatLoginCompleteDelegate);*/
	}
}

//*******************EOS Voice Chat***************************
void UMGS_OnlineSubsystem::LoginWithEOSvChat()
{
	//EOSvChatLoginCompleted.AddDynamic(this, &ThisClass::LoginEOSvChatCompleted);
	//IOnlineSubsystem* SubsystemRef = Online::GetSubsystem(this->GetWorld());
	//if (SubsystemRef)
	//{
	//	IdentityPtr = SubsystemRef->GetIdentityInterface();
	//	if (!IdentityPtr.IsValid())
	//	{
	//		MGSFunctionLibrary->DisplayDebugMessage(FString(TEXT("Identity Pointer Not valid!")), FColor::Red);
	//		EOSvChatLoginCompleted.Broadcast(false);
	//		return;
	//	}
	//	if (!IsPlayerLoggedIn())
	//	{
	//		EOSvChatLoginCompleted.Broadcast(false);
	//		return;
	//	}
	//	
	//	IVoiceChat* vChatRef = IVoiceChat::Get();
	//	if (vChatRef)
	//	{
	//		vChatUser = vChatRef->CreateUser();
	//	}

	//	if (vChatUser == nullptr)
	//	{
	//		EOSvChatLoginCompleted.Broadcast(false);
	//		return;
	//	}
	//	/*TSharedPtr<const FUniqueNetId>*/ vChatUserID = IdentityPtr->GetUniquePlayerId(0);
	//	FPlatformUserId PlatformUserID = IdentityPtr->GetPlatformUserIdFromUniqueNetId(*vChatUserID);
	//	vChatUser->Login(PlatformUserID, vChatUserID->ToString(), TEXT(""), OnChatLoginCompleteDelegate);
	//}
}
void UMGS_OnlineSubsystem::OnLoginWithEOSvChatCompleted(const FString& PlayerName, const FVoiceChatResult& Result)
{
	if (Result.IsSuccess())
	{
		UE_LOG(LogTemp, Warning, TEXT("vChat Login Success"));
		MGSFunctionLibrary->DisplayDebugMessage(FString(TEXT("vChat Login Success")), FColor::Green);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("vChat Login FAILED!!!!"));
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
			InputDeviceInfos.Add(InputDevice);
			//InputDeviceNames.Add(InputDevice.DisplayName);
		}
		for (auto OutputDevice : vChatUser->GetAvailableOutputDeviceInfos())
		{
			OutputDeviceInfos.Add(OutputDevice);
			//OutputDeviceNames.Add(OutputDevice.DisplayName);
		}
		/*InputDevices = InputDeviceNames;
		OutputDevices = OutputDeviceNames;*/
		int32 DevicID = 0;
		for (FVoiceChatDeviceInfo InputDeviceInfo : InputDeviceInfos)
		{
			InputDevices.Add(InputDeviceInfo.DisplayName);
			MGSFunctionLibrary->DisplayDebugMessage(InputDeviceInfo.DisplayName, FColor::Purple);
			MGSFunctionLibrary->DisplayDebugMessage(FString::FromInt(DevicID), FColor::Purple);
			DevicID++;
		}
		DevicID = 0;
		for (FVoiceChatDeviceInfo OutputDeviceInfo : OutputDeviceInfos)
		{
			OutputDevices.Add(OutputDeviceInfo.DisplayName);
			MGSFunctionLibrary->DisplayDebugMessage(OutputDeviceInfo.DisplayName, FColor::Purple);
			MGSFunctionLibrary->DisplayDebugMessage(FString::FromInt(DevicID), FColor::Purple);
			DevicID++;
		}
		InputDeviceNames = InputDevices;
		OutputDeviceNames = OutputDevices;
	}
}

void UMGS_OnlineSubsystem::SetInputOutputs(FString InputDeviceName, FString OutputDeviceName)
{
	IOnlineSubsystem* SubsystemEOS = Online::GetSubsystem(this->GetWorld());
	if (SubsystemEOS)
	{
		IVoiceChat* vChatRef = IVoiceChat::Get();
		if (vChatRef) vChatUser = vChatRef->CreateUser();
	}

	if (vChatUser)
	{
		FString DeviceName;
		InputDeviceInfos = vChatUser->GetAvailableInputDeviceInfos();
		OutputDeviceInfos = vChatUser->GetAvailableOutputDeviceInfos();
		for (FVoiceChatDeviceInfo InputDeviceInfo : InputDeviceInfos)
		{
			DeviceName = InputDeviceInfo.DisplayName;
			if (!InputDevices.Contains(DeviceName))
			{
				InputDevices.Add(DeviceName);
			}
		}
		for (FVoiceChatDeviceInfo OutputDeviceInfo : OutputDeviceInfos)
		{
			DeviceName = OutputDeviceInfo.DisplayName;
			if (!OutputDevices.Contains(DeviceName))
			{
				OutputDevices.Add(DeviceName);
			}
		}
		if (!InputDeviceName.IsEmpty())
		{
			vChatUser->SetInputDeviceId(InputDeviceName);
		}
		if (!OutputDeviceName.IsEmpty())
		{
			vChatUser->SetOutputDeviceId(OutputDeviceName);
		}
	}
}

//***********************EOS Stats**********************************
void UMGS_OnlineSubsystem::UpdateStats(FString StatName, int32 StatValue)
{
	IOnlineSubsystem* SubsystemRef = Online::GetSubsystem(this->GetWorld());
	if (SubsystemRef)
	{
		IdentityPtr = SubsystemRef->GetIdentityInterface();
		if (!IdentityPtr.IsValid()) return;
		IOnlineStatsPtr StatPtr = SubsystemRef->GetStatsInterface();
		if (!StatPtr.IsValid()) return;
		FOnlineStatsUserUpdatedStats UserUpdatedStats = FOnlineStatsUserUpdatedStats(IdentityPtr->GetUniquePlayerId(0).ToSharedRef());
		UserUpdatedStats.Stats.Add(StatName, FOnlineStatUpdate(StatValue, FOnlineStatUpdate::EOnlineStatModificationType::Sum));
		TArray<FOnlineStatsUserUpdatedStats> StatsArray;
		StatsArray.Add(UserUpdatedStats);
		StatPtr->UpdateStats(IdentityPtr->GetUniquePlayerId(0).ToSharedRef(), StatsArray, FOnlineStatsUpdateStatsComplete::CreateUObject(this, &ThisClass::OnUpdateStatsCompleted));
	}
}
void UMGS_OnlineSubsystem::OnUpdateStatsCompleted(const FOnlineError& Result)
{
	if (Result.bSucceeded)
	{
		MGSFunctionLibrary->DisplayDebugMessage("Stats Update SUCCESS!", FColor::Green);
		return;
	}
	MGSFunctionLibrary->DisplayDebugMessage("Stats Update SUCCESS!", FColor::Red);
}

void UMGS_OnlineSubsystem::GetStats(TArray<FString> StatNames)
{
	IOnlineSubsystem* SubsystemRef = Online::GetSubsystem(this->GetWorld());
	if (SubsystemRef)
	{
		IdentityPtr = SubsystemRef->GetIdentityInterface();
		if (!IdentityPtr.IsValid()) return;
		IOnlineStatsPtr StatPtr = SubsystemRef->GetStatsInterface();
		if (!StatPtr.IsValid()) return;
		TArray<TSharedRef<const FUniqueNetId>> UserVars;
		UserVars.Add(IdentityPtr->GetUniquePlayerId(0).ToSharedRef());
		StatPtr->QueryStats(IdentityPtr->GetUniquePlayerId(0).ToSharedRef(), UserVars, StatNames, FOnlineStatsQueryUsersStatsComplete::CreateUObject(this, &ThisClass::OnGetStatsCompleted));
		//StatPtr->GetStats(IdentityPtr->GetUniquePlayerId(0).ToSharedRef());
	}
}
void UMGS_OnlineSubsystem::OnGetStatsCompleted(const FOnlineError& Result, const TArray<TSharedRef<const FOnlineStatsUserStats>>& UserStats)
{
	if (Result.bSucceeded)
	{
		for (auto UserStat : UserStats)
		{
			for (auto Stat : UserStat->Stats)
			{
				FString KeyName = Stat.Key;
				int32 ReturnedValue;
				Stat.Value.GetValue(ReturnedValue);
				//UE_LOG(LogTemp, Warning, TEXT("Stats %s value is %d"), *KeyName, ReturnedValue);
				MGSFunctionLibrary->DisplayDebugMessage("Stat "+ KeyName+ " value is: " + FString::FromInt(ReturnedValue), FColor::Green);
			}
		}
		MGSFunctionLibrary->DisplayDebugMessage("Stats Query SUCCESS!", FColor::Green);
		return;
	}
	MGSFunctionLibrary->DisplayDebugMessage("Stats Query FAILED!", FColor::Red);
}

//************************EOS Storage*******************************
TArray<uint8> UMGS_OnlineSubsystem::ConvertSaveFileToUint(USaveGame* SaveGame)
{
	TArray<uint8> LocSaveGame;
	if (SaveGame)
	{
		UGameplayStatics::SaveGameToMemory(SaveGame, LocSaveGame);
	}
	return LocSaveGame;
}

USaveGame* UMGS_OnlineSubsystem::ConvertUintToSaveFile(TArray<uint8> ConvertedSaveFile)
{
	USaveGame* LocSaveGame = nullptr;
	if (!ConvertedSaveFile.IsEmpty())
	{
		LocSaveGame = UGameplayStatics::LoadGameFromMemory(ConvertedSaveFile);
	}
	return LocSaveGame;
}

void UMGS_OnlineSubsystem::UploadSavedData(FString FileName, TArray<uint8> DataValues)
{
	IOnlineSubsystem* SubsystemRef = Online::GetSubsystem(this->GetWorld());
	if (SubsystemRef)
	{
		IdentityPtr = SubsystemRef->GetIdentityInterface();
		if (!IdentityPtr.IsValid()) return;
		IOnlineUserCloudPtr UserCloudPtr = SubsystemRef->GetUserCloudInterface();
		if (!UserCloudPtr.IsValid()) return;
		TSharedPtr<const FUniqueNetId> UserIDRef = IdentityPtr->GetUniquePlayerId(0).ToSharedRef();
		UserCloudPtr->OnWriteUserFileCompleteDelegates.AddUObject(this, &ThisClass::OnWriteUserFileComplete);
		UserCloudPtr->WriteUserFile(*UserIDRef, FileName, DataValues);
	}
}
void UMGS_OnlineSubsystem::OnWriteUserFileComplete(bool bIsSuccessful, const FUniqueNetId& UserNetID, const FString& FileName)
{
	if (bIsSuccessful)
	{
		MGSFunctionLibrary->DisplayDebugMessage(FileName+" uploaded successfully!", FColor::Green);
		return;
	}
	MGSFunctionLibrary->DisplayDebugMessage("Uploading data FAILED!", FColor::Red);
}

void UMGS_OnlineSubsystem::DownloadSavedData(FString FileName)
{
	IOnlineSubsystem* SubsystemRef = Online::GetSubsystem(this->GetWorld());
	if (SubsystemRef)
	{
		IdentityPtr = SubsystemRef->GetIdentityInterface();
		if (!IdentityPtr.IsValid()) return;
		IOnlineUserCloudPtr UserCloudPtr = SubsystemRef->GetUserCloudInterface();
		if (!UserCloudPtr.IsValid()) return;
		TSharedPtr<const FUniqueNetId> UserIDRef = IdentityPtr->GetUniquePlayerId(0).ToSharedRef();
		UserCloudPtr->OnReadUserFileCompleteDelegates.AddUObject(this, &ThisClass::OnReadUserFileComplete);
		UserCloudPtr->ReadUserFile(*UserIDRef, FileName);
	}
}
void UMGS_OnlineSubsystem::OnReadUserFileComplete(bool bIsSuccessful, const FUniqueNetId& UserNetID, const FString& FileName)
{
	if (bIsSuccessful)
	{
		IOnlineSubsystem* SubsystemRef = Online::GetSubsystem(this->GetWorld());
		if (SubsystemRef)
		{
			IdentityPtr = SubsystemRef->GetIdentityInterface();
			if (!IdentityPtr.IsValid()) return;
			IOnlineUserCloudPtr UserCloudPtr = SubsystemRef->GetUserCloudInterface();
			if (!UserCloudPtr.IsValid()) return;
			TSharedPtr<const FUniqueNetId> UserIDRef = IdentityPtr->GetUniquePlayerId(0).ToSharedRef();
			TArray<uint8> LocFileContent;
			UserCloudPtr->GetFileContents(*UserIDRef, FileName, LocFileContent);
			OnFileContentSaveCompleted.Broadcast(bIsSuccessful, FileName, LocFileContent);
		}
		MGSFunctionLibrary->DisplayDebugMessage(FileName+" downloaded successfully!", FColor::Green);
		return;
	}
	OnFileContentSaveCompleted.Broadcast(bIsSuccessful, "", {});
	MGSFunctionLibrary->DisplayDebugMessage("Downloading data FAILED!", FColor::Red);
}