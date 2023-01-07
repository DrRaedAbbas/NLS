// Copy righted to RAED ABBAS 2023


#include "BaseGameMode.h"

#include "MGSFunctionLibrary.h"
#include "GameFramework/GameStateBase.h"

ABaseGameMode::ABaseGameMode()
{
	//bUseSeamlessTravel = true;
}

void ABaseGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	int32 NumberOfCurrentPlayers = GameState.Get()->PlayerArray.Num();
	FString NumberOfPlayersString = FString(TEXT("***Number Of Players: ") + FString::FormatAsNumber(NumberOfCurrentPlayers));

	MGSFunctionLibrary->DisplayDebugMessage(NumberOfPlayersString, FColor::Orange);
}

void ABaseGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	int32 NumberOfCurrentPlayers = GameState.Get()->PlayerArray.Num();
	FString NumberOfPlayersString = FString(TEXT("***Number Of Players: ") + FString::FormatAsNumber(NumberOfCurrentPlayers));

	MGSFunctionLibrary->DisplayDebugMessage(NumberOfPlayersString, FColor::Orange);
}
