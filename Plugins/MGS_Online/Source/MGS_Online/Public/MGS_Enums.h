// Copy righted to RAED ABBAS 2023

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EGameType : uint8
{
	NONE,
	FreeForAll UMETA(DisplayName = "Free For All"),
	DeathMatch UMETA(DisplayName = "Death Match"),
	TeamDeathMatch UMETA(DisplayName = "Team Death Match")
};

UENUM(BlueprintType)
enum class ETeams : uint8
{
	NONE UMETA(DisplayName = "No Team"),
	REDTEAM UMETA(DisplayName = "Red Team"),
	BLUETEAM UMETA(DisplayName = "Blue Team")
};