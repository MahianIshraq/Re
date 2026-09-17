#include "Game/Manager/ReAtmosphereManager.h"

#include "Actor/World/ReAtmosphere.h"
#include "Misc/AssertionMacros.h"
#include "System/ReLog.h"

AReAtmosphere* UReAtmosphereManager::GetAtmosphere() const
{
	return Atmosphere.Get();
}

void UReAtmosphereManager::RegisterAtmosphere(AReAtmosphere* NewAtmosphere)
{
	if (!NewAtmosphere)
	{
		RE_ERROR_ATMOSPHERE("RegisterAtmosphere(nullptr) is illegal.");
		check(false);

		return;
	}

	if (Atmosphere == NewAtmosphere)
	{
		RE_WARN_ATMOSPHERE("Atmosphere is already registered.");
		return;
	}

	if (Atmosphere.Get())
	{
		RE_ERROR_ATMOSPHERE("Duplicate atmosphere found.");
		check(false);

		return;
	}

	Atmosphere = NewAtmosphere;
	RE_LOG_ATMOSPHERE("Registered atmosphere.");
}

void UReAtmosphereManager::UnregisterAtmosphere()
{
	Atmosphere.Reset();
	RE_LOG_ATMOSPHERE("Unregistered atmosphere.");
}
