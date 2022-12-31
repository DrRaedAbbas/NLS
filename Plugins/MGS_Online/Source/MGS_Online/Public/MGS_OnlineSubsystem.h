// Copy righted to RAED ABBAS 2023

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"

#include "MGS_OnlineSubsystem.generated.h"

/**
 * Custom delegates
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMGSCreateSessionCompleted, bool, bWasSuccessful);
DECLARE_MULTICAST_DELEGATE_TwoParams(FMGSFindSessionsCompleted, const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessful);
DECLARE_MULTICAST_DELEGATE_OneParam(FMGSJoinSessionCompleted, EOnJoinSessionCompleteResult::Type Result);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMGSStartSessionCompleted, bool, bWasSuccessful);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMGSDestroySessionCompleted, bool, bWasSuccessful);

UCLASS()
class MGS_ONLINE_API UMGS_OnlineSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UMGS_OnlineSubsystem();

	//Functions for menus
	void CreateGameSession(int32 MaxPlayers, FString MatchType);
	void FindGameSessions(int32 MaxSearchResults = 10000);
	void JoinGameSession(const FOnlineSessionSearchResult& SessionSearchResult);
	void StartGameSession();
	void DestroyGameSession();

	//Custom delegates to broadcast
	FMGSCreateSessionCompleted MGSCreateSessionCompleted;
	FMGSFindSessionsCompleted MGSFindSessionsCompleted;
	FMGSJoinSessionCompleted MGSJoinSessionCompleted;
	FMGSStartSessionCompleted MGSStartSessionCompleted;
	FMGSDestroySessionCompleted MGSDestroySessionCompleted;

protected:
	//callbacks for subsystem's delegates
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

	//To bind subsystem callbacks
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
};
