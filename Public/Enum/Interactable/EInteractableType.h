#pragma once

#include "CoreMinimal.h"
#include "EInteractableType.generated.h"

UENUM() enum class EInteractableType : uint8
{
	None UMETA(Hidden),
	
	Item,
	NPC
};