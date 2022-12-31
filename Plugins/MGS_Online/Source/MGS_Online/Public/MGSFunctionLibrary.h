// Copy righted to RAED ABBAS 2023

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MGSFunctionLibrary.generated.h"

UCLASS()
class MGS_ONLINE_API UMGSFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "MGS|Functions Library")
	static FString DisplayDebugMessage(FString DebugMessage, FLinearColor MessageColor);
};
