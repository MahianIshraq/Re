#pragma once

#include "HAL/Platform.h"
#include "UObject/ObjectMacros.h"
#include "EReCameraView.generated.h"

UENUM() enum class EReCameraView : uint8
{
	Default, Side, Overhead
};
