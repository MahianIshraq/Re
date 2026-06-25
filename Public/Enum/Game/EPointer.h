#pragma once

#include "CoreMinimal.h"
#include "EPointer.generated.h"

UENUM() enum class EPointer : uint8
{
	None,
	
	Invisible,
	Default,
	
	Click,
	Grab,
	
	Pickup,
	Speak
};