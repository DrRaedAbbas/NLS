// Copy righted to RAED ABBAS 2023

#include "MenuButton.h"
#include "OnlineMenu.h"
#include "MGS_OnlineSubsystem.h"

#define LOCTEXT_NAMESPACE "UMG"

UMenuButton::UMenuButton()
{
	//static ConstructorHelpers::FObjectFinder<USlateWidgetStyleAsset> CustomButtonStyle(TEXT("/Game/MGS/UI/Buttons/WS_Button"));
	if (CustomButtonStyle)
	{
		SButton::FArguments ButtonDefaults;
		ButtonDefaults.ButtonStyle(CustomButtonStyle);
		WidgetStyle = *ButtonDefaults._ButtonStyle;
	}
	/*SButton::FArguments ButtonDefaults;
	ButtonDefaults.ButtonStyle(CustomButtonStyle.Object);
	WidgetStyle = *ButtonDefaults._ButtonStyle;*/

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

	//if (MainMenu) MainMenu->OnButtonReady.AddDynamic(this, &ThisClass::OnButtonReady);
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		MGS_OnlineSubsystem = GameInstance->GetSubsystem<UMGS_OnlineSubsystem>();
		/*if (MGS_OnlineSubsystem)
		{
			MGS_OnlineSubsystem->MGSCreateSessionCompleted.AddDynamic(this, &ThisClass::OnCreateSessionCompleted);
			MGS_OnlineSubsystem->MGSFindSessionsCompleted.AddDynamic(this, &ThisClass::OnFindSessionCompleted);
			MGS_OnlineSubsystem->MGSJoinSessionCompleted.AddUObject(this, &ThisClass::OnJoinSessionCompleted);
			MGS_OnlineSubsystem->MGSStartSessionCompleted.AddDynamic(this, &ThisClass::OnStartSessionCompleted);
			MGS_OnlineSubsystem->MGSDestroySessionCompleted.AddDynamic(this, &ThisClass::OnDestroySessionCompleted);
		}*/
	}
}

void UMenuButton::SetupSearchResult(FBlueprintSessionResult InSearchResult)
{
	SessionSearchResult = InSearchResult.OnlineResult;
}

void UMenuButton::OnButtonClicked()
{
	SetIsEnabled(false);
	/*if (MGS_OnlineSubsystem)
	{
		MGS_OnlineSubsystem->MGSCreateSessionCompleted.AddDynamic(this, &ThisClass::OnCreateSessionCompleted);
		MGS_OnlineSubsystem->MGSFindSessionsCompleted.AddDynamic(this, &ThisClass::OnFindSessionCompleted);
		MGS_OnlineSubsystem->MGSJoinSessionCompleted.AddUObject(this, &ThisClass::OnJoinSessionCompleted);
		MGS_OnlineSubsystem->MGSStartSessionCompleted.AddDynamic(this, &ThisClass::OnStartSessionCompleted);
		MGS_OnlineSubsystem->MGSDestroySessionCompleted.AddDynamic(this, &ThisClass::OnDestroySessionCompleted);
	}*/
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
				//MainMenu->HostButtonClicked();
				MGS_OnlineSubsystem->MGSCreateSessionCompleted.AddDynamic(this, &ThisClass::OnCreateSessionCompleted);
				MGS_OnlineSubsystem->CreateGameSession();
				break;
			}
			case EButtonType::FindButton:
			{
				//MainMenu->FindButtonClicked();
				MGS_OnlineSubsystem->MGSFindSessionsCompleted.AddDynamic(this, &ThisClass::OnFindSessionCompleted);
				MGS_OnlineSubsystem->FindGameSessions();
				break;
			}
			case EButtonType::JoinButton:
			{
				//MainMenu->JoingButtonClicked();
				//FOnlineSessionSearchResult& SessionSearchResult = SearchResult.OnlineResult;
				if (!SessionSearchResult.IsValid()) break;
				MGS_OnlineSubsystem->MGSJoinSessionCompleted.AddUObject(this, &ThisClass::OnJoinSessionCompleted);
				MGS_OnlineSubsystem->JoinGameSession(SessionSearchResult);
				break;
			}
			case EButtonType::QuitButton:
			{
				//MainMenu->QuitButtonClicked();
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
