// Copy righted to RAED ABBAS 2023

#pragma once

#include "CoreMinimal.h"
#include "MGS_OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/OnlineSessionInterface.h"

#include "OnlineMenu.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnButtonReady, bool, bButtonRead)

UCLASS()
class MGS_ONLINE_API UOnlineMenu : public UUserWidget
{
	GENERATED_BODY()

	virtual void NativePreConstruct() override;

public:
	UFUNCTION(BlueprintCallable)
	void LoadMenu();
	UFUNCTION(BlueprintCallable)
	void UnloadMenu();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "MGS")
	void SaveGame();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MGS|Lobby Settiongs")
	int32 MaxPlayers = 4;
	UPROPERTY()
	FString MatchType = FString("FreeForAll");
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MGS|Lobby Settiongs")
	EGameType GameType = EGameType::FreeForAll;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MGS|Lobby Settiongs")
	FString LevelPath = FString("/Game/NLS/Maps/Lobby");
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MGS|Lobby Settiongs")
	bool bIsDedicatedServer = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MGS|Lobby Settiongs")
	bool IsInGameMenu = false;

	UFUNCTION()
	void HostButtonClicked();
	UFUNCTION()
	void JoingButtonClicked();
	UFUNCTION()
	void FindButtonClicked();
	UFUNCTION()
	void QuitButtonClicked();

	FOnButtonReady OnButtonReady;

private:
	class UMGS_OnlineSubsystem* MGS_OnlineSubsystem;
	class UMGSFunctionLibrary* MGSFunctionLibrary;

	FOnlineSessionSearchResult SessionSearchResult;

protected:
	virtual bool Initialize() override;
	virtual void NativeDestruct() override;

	//Custom delegates callbacks
	UFUNCTION()
	void OnCreateSession(bool bWasSuccessful);
	void OnFindSessions(const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessful);
	void OnJoinSession(EOnJoinSessionCompleteResult::Type Result);
	UFUNCTION()
	void OnStartSession(bool bWasSuccessful);
	UFUNCTION()
	void OnDestroySession(bool bWasSuccessful);
};
