// Copy righted to RAED ABBAS 2023

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "FindSessionsCallbackProxy.h"
#include "VoiceChat.h"
#include "VoiceChatResult.h"

#include "MGS_OnlineSubsystem.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMGSCreateSessionCompleted, bool, bWasSuccessful);
DECLARE_MULTICAST_DELEGATE_TwoParams(FMGSFindSessionsCompleted, const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessful);
DECLARE_MULTICAST_DELEGATE_OneParam(FMGSJoinSessionCompleted, EOnJoinSessionCompleteResult::Type Result);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMGSStartSessionCompleted, bool, bWasSuccessful);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMGSDestroySessionCompleted, bool, bWasSuccessful);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEOSLoginCompleted, bool, bWasSuccessful);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEOSvChatLoginCompleted, bool, bWasSuccessful);

UCLASS()
class MGS_ONLINE_API UMGS_OnlineSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UMGS_OnlineSubsystem();

	UFUNCTION(BlueprintCallable)
	void SetGameSettings(int32 MaxPlayers, FString MatchType/*, FString LevelPath*/, bool bIsDedicatedServer);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "MGS|Online")
	FString GetCurrentPlayerName();
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "MGS|Online")
	void GetPlayerStatus(bool& IsLoggedIn, FString& OutStatus);
	UFUNCTION()
	bool IsPlayerLoggedIn();

	//Functions for menus
	UFUNCTION(BlueprintCallable, Category = "MGS|Online")
	void LoginWithEOS(FString ID, FString Token, FString LoginType);
	UFUNCTION(BlueprintCallable, Category = "MGS|Online")
	void CreateGameSession(int32 MaxPlayers, FString MatchType);
	UFUNCTION(BlueprintCallable, Category = "MGS|Online")
	void FindGameSessions(int32 MaxSearchResults = 10000);
	//UFUNCTION(BlueprintCallable, Category = "MGS|Online")
	void JoinGameSession(const FOnlineSessionSearchResult& SessionSearchResult);
	UFUNCTION(BlueprintCallable, Category = "MGS|Online")
	void StartGameSession();
	UFUNCTION(BlueprintCallable, Category = "MGS|Online")
	void DestroyGameSession();
	

	UPROPERTY(BlueprintReadOnly, Category="MGS||Online")
	TArray<FBlueprintSessionResult> SessionSearchResults;

	/*UPROPERTY(BlueprintReadOnly, Category = "MGS||Online")
	TArray<FString>InputDeviceNames;
	UPROPERTY(BlueprintReadOnly, Category = "MGS||Online")
	TArray<FString>OutputDeviceNames;*/

	//Custom delegates to broadcast
	UPROPERTY(BlueprintAssignable, Category = "MGS||Online||Delegates")
	FMGSCreateSessionCompleted MGSCreateSessionCompleted;
	FMGSFindSessionsCompleted MGSFindSessionsCompleted;
	FMGSJoinSessionCompleted MGSJoinSessionCompleted;
	UPROPERTY(BlueprintAssignable, Category = "MGS||Online||Delegates")
	FMGSStartSessionCompleted MGSStartSessionCompleted;
	UPROPERTY(BlueprintAssignable, Category = "MGS||Online||Delegates")
	FMGSDestroySessionCompleted MGSDestroySessionCompleted;
	UPROPERTY(BlueprintAssignable, Category = "MGS||Online||Delegates")
	FEOSLoginCompleted EOSLoginCompleted;
	UPROPERTY(BlueprintAssignable, Category = "MGS||Online||Delegates")
	FEOSvChatLoginCompleted EOSvChatLoginCompleted;

	//For VoiceChat
	UFUNCTION(BlueprintCallable, Category = "MGS|Online")
	void LoginWithEOSvChat();
	UFUNCTION(BlueprintCallable, Category = "MGS|Online")
	void SetEOSvChatVoiceVolmue(float NewVolume);
	UFUNCTION(BlueprintCallable, Category = "MGS|Online")
	void SetEOSvChatVoiceMute(bool bMute);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "MGS|Online")
	void GetInputOutput(TArray<FString>&InputDeviceNames, TArray<FString>&OutputDeviceNames);
	UFUNCTION(BlueprintCallable, Category = "MGS|Online")
	void SetInputOutputs(FString InputDeviceName, FString OutputDeviceName);
	UFUNCTION()
	void LoginEOSCompleted(bool bSuccess);
	UFUNCTION()
	void LoginEOSvChatCompleted(bool bSuccess);

protected:
	//callbacks for subsystem's delegates
	void OnLoginWithEOSCompleted(int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserID, const FString& Error);
	void OnLoginWithEOSvChatCompleted(const FString& PlayerName, const FVoiceChatResult& Result);
	void OnCreateSessionCompleted(FName SessionName, bool bWasSuccessful);
	void OnFindSessionsCompleted(bool bWasSuccessful);
	void OnJoinSessionCompleted(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	void OnStartSessionCompleted(FName SessionName, bool bWasSuccessful);
	void OnDestroySessionCompleted(FName SessionName, bool bWasSuccessful);

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

	bool bCreateSessionOnDestroy = false;
	int32 LastMaxPlayers;
	FString LastMatchType;

	IVoiceChatUser* vChatUser;
	TSharedPtr<const FUniqueNetId> vChatUserID;
};
