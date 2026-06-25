#pragma once

#include "Enum/Game/EPointer.h"

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "PointerManager.generated.h"

UCLASS() class RE_API UPointerManager final : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:

	// Setters

	void SetPointer(EPointer InPointer);

	// Other

	void ShowPointer();
	void HidePointer();

private:

	// Helper Functions

	void RefreshPointer();

private:

	EPointer Pointer = EPointer::None;
};