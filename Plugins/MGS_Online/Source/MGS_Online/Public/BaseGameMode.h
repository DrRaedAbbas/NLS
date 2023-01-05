// Copy righted to RAED ABBAS 2023

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"

#include "BaseGameMode.generated.h"

UCLASS()
class MGS_ONLINE_API ABaseGameMode : public AGameMode
{
	GENERATED_BODY()

	ABaseGameMode();

public:
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;

	UPROPERTY()
	int32 MaxPlayersAllowed;
	UPROPERTY()
	FString LevelPath;

protected:
	class UMGSFunctionLibrary* MGSFunctionLibrary;
	
};
