// Copy righted to RAED ABBAS 2023

#pragma once

#include "CoreMinimal.h"
#include "Components/Button.h"

#include "MenuButton.generated.h"

UENUM(BlueprintType)
enum class EButtonType : uint8
{
	NONE,
	HostButton,
	JoinButton,
	FindButton
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
	void SetupButton(class UOnlineMenu* NewMainMenu);

protected:
	UPROPERTY(EditAnywhere)
	EButtonType ButtonType = EButtonType::NONE;

	UFUNCTION()
	void OnButtonClicked();
	UFUNCTION()
	void OnButtonReady(bool bButtonReady);
};
