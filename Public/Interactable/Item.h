#pragma once

// TODO: Unfinished

class UStaticMeshComponent;

#include "CoreMinimal.h"
#include "Interactable/Interactable.h"
#include "Item.generated.h"

UCLASS
(
	HideCategories =
	(
		"Actor Tick",
		"Events"
	)
)
class RE_API AItem final : public AInteractable
{
	GENERATED_BODY()

public:

	AItem();

	// Other

	virtual void Highlight(bool bFlag) override;

private:

	// Components

	// UPROPERTY() TObjectPtr<UStaticMeshComponent> StaticMeshComponent;
	UPROPERTY(VisibleDefaultsOnly) TObjectPtr<UStaticMeshComponent> StaticMeshComponent;
};