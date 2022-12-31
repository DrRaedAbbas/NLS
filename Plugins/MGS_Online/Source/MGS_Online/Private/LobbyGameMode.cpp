// Copy righted to RAED ABBAS 2023


#include "LobbyGameMode.h"

#include "MGSFunctionLibrary.h"
#include "GameFramework/GameStateBase.h"

ALobbyGameMode::ALobbyGameMode()
{
	bUseSeamlessTravel = true;
}

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	int32 NumberOfCurrentPlayers = GameState.Get()->PlayerArray.Num();
	FString NumberOfPlayersString = FString(TEXT("***Number Of Players: ") + FString::FormatAsNumber(NumberOfCurrentPlayers));

	MGSFunctionLibrary->DisplayDebugMessage(NumberOfPlayersString, FColor::Orange);

	if (NumberOfCurrentPlayers == MaxPlayersAllowed)
	{
		if (UWorld* World = GetWorld())
		{
			LevelPath = LevelPath + FString(TEXT("?listen"));
			World->ServerTravel(LevelPath);
		}
	}
}
