// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"

#include "NLSGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class NLS_API UNLSGameInstance : public UGameInstance
{
	GENERATED_BODY()

	virtual void Init() override;
	
public:
	UFUNCTION(BlueprintCallable)
	void CreateNewSession();
	UFUNCTION(BlueprintCallable)
	void SetGameSettings();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="NLS|Lobby Settiongs")
	FName GameName = FName("My NLS Game");
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="NLS|Lobby Settiongs")
	int MaxPlayers = 4;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="NLS|Lobby Settiongs")
	bool IsDedicated = false;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="NLS|Lobby Settiongs")
	bool IsLAN = true;
	
protected:
	FOnlineSessionSettings GameSettings;
	IOnlineSubsystem* OnlineSubsystem;
	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
};
