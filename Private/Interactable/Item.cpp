#include "Interactable/Item.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/CollisionProfile.h"

AItem::AItem()
{
	PrimaryActorTick.bCanEverTick = false;
	
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	RootComponent = StaticMeshComponent;
	StaticMeshComponent->SetCollisionProfileName(UCollisionProfile::BlockAll_ProfileName);

	InteractableType = EInteractableType::Item;
	HoverPointer = EPointer::Pickup;
}

void AItem::Highlight(bool bFlag)
{
	Super::Highlight(bFlag);
	
	if (bFlag)
	{
		StaticMeshComponent->SetOverlayMaterial(M_Highlight_Overlay);
	}
	else
	{
		StaticMeshComponent->SetOverlayMaterial(nullptr);
	}
}