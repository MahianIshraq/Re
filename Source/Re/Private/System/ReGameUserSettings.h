#pragma once

#include "GameFramework/GameUserSettings.h"
#include "ReGameUserSettings.generated.h"

UCLASS() class UReGameUserSettings : public UGameUserSettings
{
	GENERATED_BODY()

public:

	virtual void SaveSettings() override;
};
