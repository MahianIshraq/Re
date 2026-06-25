#pragma once

// TODO: DIRTY

#include "Misc/Guid.h"
// #include "Enum/Interactable/Item/EItemType.h"
// #include "Struct/Interactable/Item/FCompartment.h"

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "ReSaveGame.generated.h"

UCLASS() class RE_API UReSaveGame final : public USaveGame
{
	GENERATED_BODY()

public:

	// Save Manager

	UPROPERTY(SaveGame) TSet<FGuid> PickedUpItemInstances;
	
	// Inventory Manager
	
	// UPROPERTY(SaveGame) TMap<EItemType, FCompartment> Inventory;
	
	// Player Pawn
	
	UPROPERTY(SaveGame) FTransform PlayerPawnTransform = FTransform::Identity;
	
	// Player Camera

	UPROPERTY(SaveGame) FVector PlayerCameraLocation = FVector::ZeroVector;
	UPROPERTY(SaveGame) FRotator PlayerCameraRotation = FRotator::ZeroRotator;
};