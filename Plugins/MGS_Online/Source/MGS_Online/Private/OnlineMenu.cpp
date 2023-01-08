// Copy righted to RAED ABBAS 2023


#include "OnlineMenu.h"

#include "MenuButton.h"
#include "MGSFunctionLibrary.h"
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
	RemoveFromParent();
}

void UOnlineMenu::LoadGame_Implementation()
{
}

void UOnlineMenu::SaveGame_Implementation()
{
}

void UOnlineMenu::OnCreateSession(bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		MGSFunctionLibrary->DisplayDebugMessage(FString(TEXT("Game Session created!")), FColor::Green);
		UnloadMenu();
		MGS_OnlineSubsystem->TravelToMap(LevelPath);
	}
	else
	{
		MGSFunctionLibrary->DisplayDebugMessage(FString(TEXT("Session was not created!")), FColor::Red);
	}
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
				UnloadMenu();
			}
		}
		else
		{
			MGSFunctionLibrary->DisplayDebugMessage(FString(TEXT("CANNOT Joing Game!")), FColor::Red);
		}
	}
	else
	{
		MGSFunctionLibrary->DisplayDebugMessage(FString(TEXT("Subsystem Does Not Exist!")), FColor::Red);
	}
}

void UOnlineMenu::OnFindSessions(const TArray<FBlueprintSessionResult>& SessionResults, bool bWasSuccessful)
{
	
}

void UOnlineMenu::OnStartSession(bool bWasSuccessful)
{

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
				UnloadMenu();
			}
		}
	}
	if (!IsInGameMenu)
	{
		UKismetSystemLibrary::QuitGame(this, GetOwningPlayer(), EQuitPreference::Quit, false);
	}
}
