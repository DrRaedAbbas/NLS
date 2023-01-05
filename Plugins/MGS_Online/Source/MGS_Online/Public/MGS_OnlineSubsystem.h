// Copy righted to RAED ABBAS 2023

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "FindSessionsCallbackProxy.h"
#include "VoiceChat.h"
#include "VoiceChatResult.h"
#include "GameFramework/SaveGame.h"
#include "Interfaces/OnlineStatsInterface.h"

#include "MGS_OnlineSubsystem.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMGSCreateSessionCompleted, bool, bWasSuccessful);
DECLARE_MULTICAST_DELEGATE_TwoParams(FMGSFindSessionsCompleted, const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessful);
DECLARE_MULTICAST_DELEGATE_OneParam(FMGSJoinSessionCompleted, EOnJoinSessionCompleteResult::Type Result);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMGSStartSessionCompleted, bool, bWasSuccessful);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMGSDestroySessionCompleted, bool, bWasSuccessful);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEOSLoginCompleted, bool, bWasSuccessful);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEOSvChatLoginCompleted, bool, bWasSuccessful);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnFileContentSaveCompleted, bool, bWasSuccessful, FString, FileName, const TArray<uint8>&, FileContents);

UCLASS()
class MGS_ONLINE_API UMGS_OnlineSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UMGS_OnlineSubsystem();

	////////JUST TESTING CREATING WILD CARD
	UFUNCTION(BlueprintCallable, CustomThunk, meta = (DisplayName="test function3", Keywords = "testkeyword", MapParam = "Options", MapKeyParam = "Output", AutoCreateRefTerm = "Options"), Category = "MyProject")
	static void TestFunction3(const TMap<int32, int32>& Options, int32& Output);
	DECLARE_FUNCTION(execTestFunction3)
	{
		// add some code to get params
	}



	/**
	 * Set the session settings. The majority of these settings are set automatically but they will be exposed to blueprint in future releases.
	 * @param MaxPlayers The maximum allowed players to join the session.
	 * @param MatchType FreeForAll, DeathMatch or TeamDeathMatch match type (also known as game mode).
	 * @param bIsDedicatedServer should generally be set to false unless a Dedicated server is running.
	 */
	UFUNCTION(BlueprintCallable)
	void SetGameSettings(int32 MaxPlayers, FString MatchType, bool bIsDedicatedServer);
	/**
	 * Create a new game session. Note this is not to start a game or travel to a new map(level).
	 * @param MaxPlayers The maximum allowed players to join the session.
	 * @param MatchType FreeForAll, DeathMatch or TeamDeathMatch match type (also known as game mode).
	 */
	UFUNCTION(BlueprintCallable, Category = "MGS|Online")
	void CreateGameSession(int32 MaxPlayers, FString MatchType);
	/**
	 * Find all available game sessions.
	 * @param MaxSearchResults The maximum number of search results. Set to a high value during testing/debug.
	 */
	UFUNCTION(BlueprintCallable, Category = "MGS|Online")
	void FindGameSessions(int32 MaxSearchResults = 10000);
	void JoinGameSession(const FOnlineSessionSearchResult& SessionSearchResult);
	UFUNCTION(BlueprintCallable, Category = "MGS|Online")
	void StartGameSession();
	UFUNCTION(BlueprintCallable, Category = "MGS|Online")
	void DestroyGameSession();

	UPROPERTY(BlueprintReadOnly, Category="MGS||Online")
	TArray<FBlueprintSessionResult> SessionSearchResults;

	//Custom delegates to broadcast
	UPROPERTY(BlueprintAssignable, Category = "MGS||Online||Delegates")
	FMGSCreateSessionCompleted MGSCreateSessionCompleted;
	FMGSFindSessionsCompleted MGSFindSessionsCompleted;
	FMGSJoinSessionCompleted MGSJoinSessionCompleted;
	UPROPERTY(BlueprintAssignable, Category = "MGS||Online||Delegates")
	FMGSStartSessionCompleted MGSStartSessionCompleted;
	UPROPERTY(BlueprintAssignable, Category = "MGS||Online||Delegates")
	FMGSDestroySessionCompleted MGSDestroySessionCompleted;
	
	//EOS
	/**
	 * To login using EOS.
	 * @param ID The login ID if known. This should be blank if accountportal type is used!.
	 * @param Token The login token, or password, if known, This should be blank if accountportal type is used!.
	 * @param LoginType is the method used to log in. accountportal is set by default.
	 */
	UFUNCTION(BlueprintCallable, Category = "MGS|EOS|Lobby")
	void LoginWithEOS(FString ID, FString Token, FString LoginType);
	/**
	 * Returns if the local player has logged in.
	 */
	UFUNCTION()
	bool IsPlayerLoggedIn();
	/**
	 * Returns player's status.
	 * @param IsLoggedIn Is player logged in into EOS.
	 * @param OutStatus returns the actual status as a string text.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "MGS|EOS|Lobby")
	void GetPlayerStatus(bool& IsLoggedIn, FString& OutStatus);
	/**
	 * Returns the local player name.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "MGS|EOS|Lobby")
	FString GetCurrentPlayerName();

	UPROPERTY(BlueprintAssignable, Category = "MGS|EOS|Lobby|Delegates")
	FEOSLoginCompleted EOSLoginCompleted;
	
	//EOS VoiceChat
	UFUNCTION(BlueprintCallable, Category = "MGS|EOS|vChat")
	void LoginWithEOSvChat();
	UFUNCTION(BlueprintCallable, Category = "MGS|EOS|vChat")
	void SetEOSvChatVoiceVolmue(float NewVolume);
	UFUNCTION(BlueprintCallable, Category = "MGS|EOS|vChat")
	void SetEOSvChatVoiceMute(bool bMute);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "MGS|EOS|vChat")
	void GetInputOutput(TArray<FString>&InputDeviceNames, TArray<FString>&OutputDeviceNames);
	UFUNCTION(BlueprintCallable, Category = "MGS|EOS|vChat")
	void SetInputOutputs(FString InputDeviceName, FString OutputDeviceName);
	UFUNCTION()
	void LoginEOSCompleted(bool bSuccess);
	UFUNCTION()
	void LoginEOSvChatCompleted(bool bSuccess);

	UPROPERTY(BlueprintReadWrite, Category = "MGS|EOS|vChat")
	TArray<FString> InputDevices;
	UPROPERTY(BlueprintReadWrite, Category = "MGS|EOS|vChat")
	TArray<FString> OutputDevices;
	//UPROPERTY(BlueprintReadWrite, Category = "MGS|EOS|vChat")
	TArray<FVoiceChatDeviceInfo> InputDeviceInfos;
	//UPROPERTY(BlueprintReadWrite, Category = "MGS|EOS|vChat")
	TArray<FVoiceChatDeviceInfo> OutputDeviceInfos;

	UPROPERTY(BlueprintAssignable, Category = "MGS|EOS|Lobby|Delegates")
	FEOSvChatLoginCompleted EOSvChatLoginCompleted;

	//EOS Stats
	UFUNCTION(BlueprintCallable, Category = "MGS|EOS|Stats")
	void UpdateStats(FString StatName, int32 StatValue);
	UFUNCTION(BlueprintCallable, Category = "MGS|EOS|Stats")
	void GetStats(TArray<FString> StatNames);

	//EOS Storage
	UFUNCTION(BlueprintCallable, Category = "MGS|EOS|Storage")
	TArray<uint8> ConvertSaveFileToUint(USaveGame* SaveGame);
	UFUNCTION(BlueprintCallable, Category = "MGS|EOS|Storage")
	USaveGame* ConvertUintToSaveFile(TArray<uint8> ConvertedSaveFile);
	UFUNCTION(BlueprintCallable, Category = "MGS|EOS|Storage")
	void UploadSavedData(FString FileName, TArray<uint8> DataValues);
	UFUNCTION(BlueprintCallable, Category = "MGS|EOS|Storage")
	void DownloadSavedData(FString FileName);

	UPROPERTY(BlueprintAssignable, Category = "MGS|EOS|Storage|Delegates")
	FOnFileContentSaveCompleted OnFileContentSaveCompleted;

protected:
	//callbacks for subsystem's delegates
	void OnLoginWithEOSCompleted(int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserID, const FString& Error);
	void OnLoginWithEOSvChatCompleted(const FString& PlayerName, const FVoiceChatResult& Result);
	void OnCreateSessionCompleted(FName SessionName, bool bWasSuccessful);
	void OnFindSessionsCompleted(bool bWasSuccessful);
	void OnJoinSessionCompleted(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	void OnStartSessionCompleted(FName SessionName, bool bWasSuccessful);
	void OnDestroySessionCompleted(FName SessionName, bool bWasSuccessful);
	void OnUpdateStatsCompleted(const FOnlineError& Result);
	void OnGetStatsCompleted(const FOnlineError& Result, const TArray<TSharedRef<const FOnlineStatsUserStats>> &UserStats);
	void OnWriteUserFileComplete(bool bIsSuccessful, const FUniqueNetId& UserNetID, const FString& FileName);
	void OnReadUserFileComplete(bool bIsSuccessful, const FUniqueNetId& UserNetID, const FString& FileName);

private:
	IOnlineSessionPtr SessionInterface;
	TSharedPtr<FOnlineSessionSettings> SessionSettings;
	class UMGSFunctionLibrary* MGSFunctionLibrary;
	TSharedPtr<FOnlineSessionSearch> SessionSearch;
	IOnlineIdentityPtr IdentityPtr;

	//To bind subsystem callbacks
	FOnLoginCompleteDelegate OnLoginCompleteDelegate;
	FDelegateHandle LoginHandle;
	FOnVoiceChatLoginCompleteDelegate OnChatLoginCompleteDelegate;
	//FDelegateHandle vChatLoginHandle;
	FOnCreateSessionCompleteDelegate CreateSessionCompleteDelegate;
	FDelegateHandle CreateSessionHandle;
	FOnFindSessionsCompleteDelegate FindSessionsCompleteDelegate;
	FDelegateHandle FindSessionsHandle;
	FOnJoinSessionCompleteDelegate JoinSessionCompleteDelegate;
	FDelegateHandle JoinSessionHandle;
	FOnStartSessionCompleteDelegate StartSessionCompleteDelegate;
	FDelegateHandle StartSessionHandle;
	FOnDestroySessionCompleteDelegate DestroySessionCompleteDelegate;
	FDelegateHandle DestroySessionHandle;
	/*FOnlineStatsUpdateStatsComplete StatsUpdateCompleteDelegate;
	FDelegateHandle StatsUpdateHandle;*/

	bool bCreateSessionOnDestroy = false;
	int32 LastMaxPlayers;
	FString LastMatchType;

	IVoiceChatUser* vChatUser;
	TSharedPtr<const FUniqueNetId> vChatUserID;
};
