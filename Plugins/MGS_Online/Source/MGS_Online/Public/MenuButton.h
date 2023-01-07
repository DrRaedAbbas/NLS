// Copy righted to RAED ABBAS 2023

#pragma once

#include "CoreMinimal.h"
#include "FindSessionsCallbackProxy.h"
#include "Components/Button.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionInterface.h"

#include "MenuButton.generated.h"

UENUM(BlueprintType)
enum class EButtonType : uint8
{
	NONE,
	HostButton,
	JoinButton,
	FindButton,
	QuitButton
};

UCLASS()
class MGS_ONLINE_API UMenuButton : public UButton
{
	GENERATED_BODY()

	UMenuButton();
	
protected:
	
#if WITH_EDITOR
	virtual const FText GetPaletteCategory() override;
#endif

public:
	class UOnlineMenu* MainMenu;
	void SetupButton(UOnlineMenu* NewMainMenu);
	UFUNCTION(BlueprintCallable, Category = "MGS|Online|Buttons")
	void SetupSearchResult(FBlueprintSessionResult InSearchResult);
	UPROPERTY(EditAnywhere, Category = "MGS|Online|Buttons")
	USlateWidgetStyleAsset* CustomButtonStyle;

protected:
	class UMGS_OnlineSubsystem* MGS_OnlineSubsystem;

	FOnlineSessionSearchResult SessionSearchResult;

	UPROPERTY(EditAnywhere, Category = "MGS|Online|Buttons")
	EButtonType ButtonType = EButtonType::NONE;

	UFUNCTION()
	void OnButtonClicked();
	UFUNCTION()
	void OnButtonReady(bool bButtonReady);

	UFUNCTION()
	void OnCreateSessionCompleted(bool bWasSuccessful);
	UFUNCTION()
	void OnFindSessionCompleted(const TArray<FBlueprintSessionResult>& SessionResults, bool bWasSuccessful);
	void OnJoinSessionCompleted(EOnJoinSessionCompleteResult::Type Result);
	UFUNCTION()
	void OnStartSessionCompleted(bool bWasSuccessful);
	UFUNCTION()
	void OnDestroySessionCompleted(bool bWasSuccessful);
};
