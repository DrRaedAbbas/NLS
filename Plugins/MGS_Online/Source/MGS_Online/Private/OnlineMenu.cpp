// Copy righted to RAED ABBAS 2023


#include "OnlineMenu.h"

#include "MenuButton.h"
#include "MGSFunctionLibrary.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"
#include "Blueprint/WidgetTree.h"
#include "GameFramework/GameModeBase.h"
#include "Kismet/KismetSystemLibrary.h"

void UOnlineMenu::NativePreConstruct()
{
	Super::NativePreConstruct();
}

bool UOnlineMenu::Initialize()
{
	if (!Super::Initialize()) return false;

	return true;
}

void UOnlineMenu::NativeDestruct()
{
	UnloadMenu();
	Super::NativeDestruct();
}

void UOnlineMenu::LoadMenu()
{
	AddToViewport();
	SetVisibility(ESlateVisibility::Visible);
	bIsFocusable = true;
	//OnButtonReady.Broadcast(true);

	if (UWorld* World = GetWorld())
	{
		if (APlayerController* PlayerController = World->GetFirstPlayerController())
		{
			FInputModeUIOnly InputMode;
			InputMode.SetWidgetToFocus(TakeWidget());
			InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			PlayerController->SetInputMode(InputMode);
			PlayerController->SetShowMouseCursor(true);
			LoadGame();
		}

		if (UGameInstance* GameInstance = GetGameInstance())
		{
			MGS_OnlineSubsystem = GameInstance->GetSubsystem<UMGS_OnlineSubsystem>();
		}

		WidgetTree->ForEachWidget([&](UWidget* Widget)
			{
				if (UMenuButton* FoundButton = Cast<UMenuButton>(Widget))
				{
					FoundButton->SetupButton(this);
				}
			});
	}

	if (MGS_OnlineSubsystem)
	{
		MGS_OnlineSubsystem->MGSCreateSessionCompleted.AddDynamic(this, &ThisClass::OnCreateSession);
		MGS_OnlineSubsystem->MGSFindSessionsCompleted.AddDynamic(this, &ThisClass::OnFindSessions);
		MGS_OnlineSubsystem->MGSJoinSessionCompleted.AddUObject(this, &ThisClass::OnJoinSession);
		MGS_OnlineSubsystem->MGSStartSessionCompleted.AddDynamic(this, &ThisClass::OnStartSession);
		MGS_OnlineSubsystem->MGSDestroySessionCompleted.AddDynamic(this, &ThisClass::OnDestroySession);
	}
}

void UOnlineMenu::UnloadMenu()
{
	if (UWorld* World = GetWorld())
	{
		if (APlayerController* PlayerController = World->GetFirstPlayerController())
		{
			FInputModeGameOnly InputMode;
			PlayerController->SetInputMode(InputMode);
			PlayerController->SetShowMouseCursor(false);
			SaveGame();
		}
	}
	//OnButtonReady.Broadcast(true);
	RemoveFromParent();
}

void UOnlineMenu::LoadGame_Implementation()
{
}

void UOnlineMenu::SaveGame_Implementation()
{
}

void UOnlineMenu::HostButtonClicked()
{
	//MGSFunctionLibrary->DisplayDebugMessage(FString(TEXT("Host button clicked")), FColor::Cyan);
	/*switch (GameType)
	{
		case EGameType::FreeForAll:
			{
				MatchType = FString("FreeForAll");
				break;
			}
	case EGameType::DeathMatch:
			{
				MatchType = FString("FreeForAll");
				break;
			}
	case EGameType::TeamDeathMatch:
			{
				MatchType = FString("FreeForAll");
				break;
			}
	case EGameType::NONE:
			{
				MatchType = FString("FreeForAll");
				break;
			}
	}*/
	
	//if (MGS_OnlineSubsystem)
	//{
	//	MGS_OnlineSubsystem->SetGameSettings(ServerName, MaxPlayers, MatchType/*, LevelPath*/, bIsDedicatedServer);
	//	MGS_OnlineSubsystem->CreateGameSession(/*MaxPlayers, MatchType*/);
	//}
	//OnButtonReady.Broadcast(true);
}
void UOnlineMenu::OnCreateSession(bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		MGSFunctionLibrary->DisplayDebugMessage(FString(TEXT("Game Session created!")), FColor::Green);
		UnloadMenu();
		MGS_OnlineSubsystem->TravelToMap(LevelPath);
		//if (UWorld* World = GetWorld())
		//{
		//	/*LevelPath = LevelPath + FString(TEXT("?listen"));*/
		//	UnloadMenu();
		//	//OnButtonReady.Broadcast(true);
		//	World->ServerTravel(LevelPath);
		//}
	}
	else
	{
		MGSFunctionLibrary->DisplayDebugMessage(FString(TEXT("Session was not created!")), FColor::Red);
		//OnButtonReady.Broadcast(true);
	}
}

void UOnlineMenu::JoingButtonClicked()
{
	//MGSFunctionLibrary->DisplayDebugMessage(FString(TEXT("Join button clicked")), FColor::Cyan);
	//
	//if (!SessionSearchResult.IsValid())
	//{
	//	MGSFunctionLibrary->DisplayDebugMessage(FString(TEXT("No Games. Please try Find Game")), FColor::Red);
	//	//OnButtonReady.Broadcast(true);
	//	return;
	//}

	/*if(MGS_OnlineSubsystem) 
	{
		MGS_OnlineSubsystem->JoinGameSession(SessionSearchResult);
	}*/
}
void UOnlineMenu::OnJoinSession(EOnJoinSessionCompleteResult::Type Result)
{
	if(IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get())
	{
		IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface();
		if (SessionInterface.IsValid())
		{
			FString Address;
			SessionInterface->GetResolvedConnectString(NAME_GameSession, Address);
			APlayerController* PlayerController = GetGameInstance()->GetFirstLocalPlayerController();
			if (PlayerController)
			{
				PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
				//OnButtonReady.Broadcast(true);
				UnloadMenu();
			}
		}
		else
		{
			MGSFunctionLibrary->DisplayDebugMessage(FString(TEXT("CANNOT Joing Game!")), FColor::Red);
			//OnButtonReady.Broadcast(true);
		}
	}
	else
	{
		MGSFunctionLibrary->DisplayDebugMessage(FString(TEXT("Subsystem Does Not Exist!")), FColor::Red);
		//OnButtonReady.Broadcast(true);
	}
}

void UOnlineMenu::FindButtonClicked()
{
	/*MGSFunctionLibrary->DisplayDebugMessage(FString(TEXT("Find button clicked")), FColor::Cyan);
	if (MGS_OnlineSubsystem)
	{
		MGS_OnlineSubsystem->FindGameSessions();
	}*/
}
void UOnlineMenu::OnFindSessions(const TArray<FBlueprintSessionResult>& SessionResults, bool bWasSuccessful)
{
	//OnButtonReady.Broadcast(true);
	//if (MGS_OnlineSubsystem == nullptr) return;

	//MGSFunctionLibrary->DisplayDebugMessage(FString(TEXT("Finding Game Sessions!")), FColor::Cyan);
	//for (auto Result : SessionResults)
	//{
	//	if(Result.Session.NumOpenPublicConnections <= 0) continue;
	//	if(Result.Session.NumOpenPublicConnections <= MaxPlayers) continue;
	//	FString FoundGameType;
	//	Result.Session.SessionSettings.Get(FName("MatchType"), FoundGameType);
	//	
	//	if (FoundGameType == MatchType)
	//	{
	//		SessionSearchResult = Result;
	//		MGSFunctionLibrary->DisplayDebugMessage(FString(TEXT("Game Session Found")), FColor::Green);
	//		//OnButtonReady.Broadcast(true);
	//		break;
	//	}
	//}
}

void UOnlineMenu::QuitButtonClicked()
{
	/*MGSFunctionLibrary->DisplayDebugMessage(FString(TEXT("Quit button clicked")), FColor::Cyan);
	if (MGS_OnlineSubsystem)
	{
		MGS_OnlineSubsystem->DestroyGameSession();
	}*/
}

void UOnlineMenu::OnStartSession(bool bWasSuccessful)
{
	//OnButtonReady.Broadcast(true);
}

void UOnlineMenu::OnDestroySession(bool bWasSuccessful)
{
	UWorld* World = GetWorld();
	if (World)
	{
		AGameModeBase* GameMode = World->GetAuthGameMode();
		if (GameMode)
		{
			GameMode->ReturnToMainMenuHost();
		}
		else
		{
			if(APlayerController* PlayerController = GetGameInstance()->GetFirstLocalPlayerController())
			{
				PlayerController->ClientReturnToMainMenuWithTextReason(FText());
				//OnButtonReady.Broadcast(true);
				UnloadMenu();
			}
		}
	}
	if (!IsInGameMenu)
	{
		UKismetSystemLibrary::QuitGame(this, GetOwningPlayer(), EQuitPreference::Quit, false);
	}
}
