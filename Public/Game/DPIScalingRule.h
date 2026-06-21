#pragma once

#include "CoreMinimal.h"
#include "Engine/DPICustomScalingRule.h"
#include "DPIScalingRule.generated.h"

UCLASS() class RE_API UDPIScalingRule final : public UDPICustomScalingRule
{
	GENERATED_BODY()

public:

	virtual float GetDPIScaleBasedOnSize(FIntPoint Size) const override;
};