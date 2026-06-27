#pragma once

// TODO: Unfinished

class APlayerController;
class AActor;
class AInteractable;

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "InteractionManager.generated.h"

UCLASS() class RE_API UInteractionManager final : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:

	void TickInteraction(const APlayerController* InPlayerController);
	bool HoveredInteractableExists();
	void Interact();
	void Interact(AActor* InActor);
	void Inspect();
	void Inspect(AActor* InActor);
	
private:
	
	virtual void Initialize(FSubsystemCollectionBase& InCollection) override;

	// Helper Functions

	void SetHoveredInteractable(AInteractable* InInteractable);
	void ItemInteract(AInteractable* InInteractable);
	void NPCInteract(const AInteractable* InInteractable);

private:

	// Properties

	inline static constexpr float InteractionRadius = 500.0f;

	// Data

	TWeakObjectPtr<AInteractable> HoveredInteractable;

	// Helper Variables

	bool bBlockInteractionThisTick = false;
};