// Copy righted to RAED ABBAS 2023

#include "MenuButton.h"
#include "OnlineMenu.h"
#include "MGS_OnlineSubsystem.h"
#include "Components/TextBlock.h"

#define LOCTEXT_NAMESPACE "UMG"

UMenuButton::UMenuButton()
{
	OnClicked.AddDynamic(this, &ThisClass::OnButtonClicked);
}

#if WITH_EDITOR
const FText UMenuButton::GetPaletteCategory()
{
	return LOCTEXT("", "MGS");
}
#endif
#undef LOCTEXT_NAMESPACE

void UMenuButton::SetupButton(UOnlineMenu* NewMainMenu)
{
	if (NewMainMenu)
	{
		MainMenu = NewMainMenu;
	}
	
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		MGS_OnlineSubsystem = GameInstance->GetSubsystem<UMGS_OnlineSubsystem>();
	}

	if (CustomButtonStyle)
	{
		SButton::FArguments ButtonDefaults;
		ButtonDefaults.ButtonStyle(CustomButtonStyle);
		WidgetStyle = *ButtonDefaults._ButtonStyle;
	}
}

void UMenuButton::SetupSearchResult(FBlueprintSessionResult InSearchResult)
{
	SessionSearchResult = InSearchResult.OnlineResult;
}

void UMenuButton::OnButtonClicked()
{
	SetIsEnabled(false);
	
	if (!MGS_OnlineSubsystem)
	{
		if (UGameInstance* GameInstance = GetGameInstance())
			MGS_OnlineSubsystem = GameInstance->GetSubsystem<UMGS_OnlineSubsystem>();
	}

	if (MGS_OnlineSubsystem)
	{
		switch (ButtonType)
		{
			case EButtonType::NONE:
			{
				break;
			}
			case EButtonType::HostButton:
			{
				MGS_OnlineSubsystem->MGSCreateSessionCompleted.AddDynamic(this, &ThisClass::OnCreateSessionCompleted);
				MGS_OnlineSubsystem->CreateGameSession();
				break;
			}
			case EButtonType::FindButton:
			{
				MGS_OnlineSubsystem->MGSFindSessionsCompleted.AddDynamic(this, &ThisClass::OnFindSessionCompleted);
				MGS_OnlineSubsystem->FindGameSessions();
				break;
			}
			case EButtonType::JoinButton:
			{
				if (!SessionSearchResult.IsValid()) break;
				MGS_OnlineSubsystem->MGSJoinSessionCompleted.AddUObject(this, &ThisClass::OnJoinSessionCompleted);
				MGS_OnlineSubsystem->JoinGameSession(SessionSearchResult);
				break;
			}
			case EButtonType::QuitButton:
			{
				MGS_OnlineSubsystem->DestroyGameSession();
				break;
			}

			default:
			{
				break;
			}
		}
	}
}

void UMenuButton::OnButtonReady(bool bButtonReady)
{
	SetIsEnabled(bButtonReady);
}

void UMenuButton::OnCreateSessionCompleted(bool bWasSuccessful)
{
	OnButtonReady(true);
	MGS_OnlineSubsystem->MGSCreateSessionCompleted.RemoveDynamic(this, &ThisClass::OnCreateSessionCompleted);
}

void UMenuButton::OnFindSessionCompleted(const TArray<FBlueprintSessionResult>& SessionResults, bool bWasSuccessful)
{
	OnButtonReady(true);
	MGS_OnlineSubsystem->MGSFindSessionsCompleted.RemoveDynamic(this, &ThisClass::OnFindSessionCompleted);
}

void UMenuButton::OnJoinSessionCompleted(EOnJoinSessionCompleteResult::Type Result)
{
	OnButtonReady(true);
}

void UMenuButton::OnStartSessionCompleted(bool bWasSuccessful)
{
	OnButtonReady(true);
}

void UMenuButton::OnDestroySessionCompleted(bool bWasSuccessful)
{
	OnButtonReady(true);
}
