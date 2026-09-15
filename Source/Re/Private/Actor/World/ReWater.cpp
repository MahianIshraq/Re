#include "Actor/World/ReWater.h"

#include "Components/PrimitiveComponent.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/EngineBaseTypes.h"
#include "Engine/EngineTypes.h"
#include "Engine/StaticMesh.h"
#include "Math/Vector.h"
#include "System/ReAssetManager.h"
#include "UObject/Object.h"

AReWater::AReWater()
{
	PrimaryActorTick.bCanEverTick = false;

	StaticMeshComponent		= CreateDefaultSubobject<UStaticMeshComponent>("StaticMeshComponent");
	RootComponent			= StaticMeshComponent;

	StaticMeshComponent->SetMobility			(EComponentMobility::Static);
	StaticMeshComponent->SetCollisionEnabled	(ECollisionEnabled::NoCollision);
	StaticMeshComponent->SetCastShadow			(false);
}

void AReWater::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	SetActorScale3D
	(
		FVector(Scale, Scale, 1.0f)
	);

	if (StaticMeshComponent->GetStaticMesh())
	{
		return;
	}

	StaticMeshComponent->SetStaticMesh
	(
		UReAssetManager::LoadAssetSync<UStaticMesh>("SM_Water")
	);
}
