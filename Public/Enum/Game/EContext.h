#pragma once

#include "CoreMinimal.h"
#include "EContext.generated.h"

UENUM() enum class EContext : uint8
{
	None,
	
	Overworld,
	Conversation,
	Combat,
	
	MainMenu,
	InventoryMenu,
	CompartmentMenu,

	Popup,
	LoadingScreen
};