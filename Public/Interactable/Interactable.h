#pragma once

#include "Enum/Interactable/EInteractableType.h"
#include "Enum/Game/EPointer.h"

class UMaterialInterface;

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactable.generated.h"

UCLASS
(
	Abstract,
	HideCategories =
	(
		Rendering,
		HLOD,
		Replication,
		Collision,
		Physics,
		Networking,
		Input,
		Actor,
		WorldPartition,
		LevelInstance,
		Cooking,
		DataLayers
	)
)
class RE_API AInteractable : public AActor
{
	GENERATED_BODY()

public:

	AInteractable();

	// Getters
	
	EInteractableType GetInteractableType() const;
	virtual EPointer GetHoverPointer();

	// Other
	
	virtual void Highlight(bool bFlag);

protected:

	// Properties

	EInteractableType InteractableType = EInteractableType::None;
	EPointer HoverPointer = EPointer::Click;

	// Assets

	UPROPERTY(Transient) TObjectPtr<UMaterialInterface> M_Highlight_Overlay;
};