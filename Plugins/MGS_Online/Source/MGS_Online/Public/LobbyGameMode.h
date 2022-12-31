// Copy righted to RAED ABBAS 2023

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"

#include "LobbyGameMode.generated.h"

UCLASS()
class MGS_ONLINE_API ALobbyGameMode : public AGameMode
{
	GENERATED_BODY()

	ALobbyGameMode();

public:
	virtual void PostLogin(APlayerController* NewPlayer) override;

	UPROPERTY()
	int32 MaxPlayersAllowed;
	UPROPERTY()
	FString LevelPath;

protected:
	class UMGSFunctionLibrary* MGSFunctionLibrary;

	

};
