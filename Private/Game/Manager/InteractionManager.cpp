#include "Game/Manager/InteractionManager.h"
#include "Engine/GameInstance.h"
#include "Game/Manager/ContextManager.h"
#include "Game/Manager/PointerManager.h"
#include "Engine/HitResult.h"
#include "GameFramework/PlayerController.h"
#include "Interactable/Interactable.h"

bool UInteractionManager::HoveredInteractableExists()
{
	return HoveredInteractable.IsValid();
}

void UInteractionManager::TickInteraction(const APlayerController* InPlayerController)
{
	
}

void UInteractionManager::Interact()
{
	// Null is legal here, so no assertion.
	
	if (HoveredInteractable.IsValid())
	{
		Interact(HoveredInteractable.Get());
	}
}

void UInteractionManager::Interact(AActor* InActor)
{
	AInteractable* Interactable = Cast<AInteractable>(InActor);
	check(Interactable);
	
	switch (Interactable->GetInteractableType())
	{
		case EInteractableType::Item:
		{
			ItemInteract(Interactable);
			break;
		}

		case EInteractableType::NPC:
		{
			NPCInteract(Interactable);
			break;	
		}

		default:
		{
			break;
		}
	}
}

void UInteractionManager::Inspect()
{
	// Null is legal here, so no assertion.
	
	if (HoveredInteractable.IsValid())
	{
		Inspect(HoveredInteractable.Get());
	}
}

void UInteractionManager::Inspect(AActor* InActor)
{
	
}

void UInteractionManager::Initialize(FSubsystemCollectionBase& InCollection)
{
	Super::Initialize(InCollection);
	
	InCollection.InitializeDependency(UContextManager::StaticClass());
	InCollection.InitializeDependency(UPointerManager::StaticClass());
}

void UInteractionManager::SetHoveredInteractable(AInteractable* InInteractable)
{
	if (InInteractable == HoveredInteractable)
	{
		return;
	}

	if (HoveredInteractable.IsValid())
	{
		HoveredInteractable->Highlight(false);
	}

	HoveredInteractable = InInteractable;
	
	if (HoveredInteractable.IsValid())
	{
		HoveredInteractable->Highlight(true);
	}
}

void UInteractionManager::ItemInteract(AInteractable* InInteractable)
{
	
}

void UInteractionManager::NPCInteract(const AInteractable* InInteractable)
{
	
}