// Copy righted to RAED ABBAS 2023

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FMGS_OnlineModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
