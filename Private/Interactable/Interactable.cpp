#include "Interactable/Interactable.h"
#include "UObject/ConstructorHelpers.h"
#include "Materials/MaterialInterface.h"
#include "Struct/Asset/FMaterialPaths.h"

AInteractable::AInteractable()
{
	PrimaryActorTick.bCanEverTick = false;
	M_Highlight_Overlay = ConstructorHelpers::FObjectFinder<UMaterialInterface>(FMaterialPaths::M_Highlight_Overlay).Object;
}

EInteractableType AInteractable::GetInteractableType() const
{
	return InteractableType;
}

EPointer AInteractable::GetHoverPointer()
{
	return HoverPointer;
}

void AInteractable::Highlight(bool bFlag)
{
	
}