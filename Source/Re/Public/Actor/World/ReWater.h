#pragma once

#include "GameFramework/Actor.h"
#include "Math/Transform.h"
#include "UObject/ObjectMacros.h"
#include "UObject/ObjectPtr.h"
#include "ReWater.generated.h"

struct	FPropertyChangedEvent;
class	UStaticMeshComponent;

UCLASS
(
	DisplayName = "Water",

	HideCategories =
	(
		Rendering, HLOD, Replication, Collision, Physics, Networking, Input, Actor, WorldPartition, LevelInstance, Cooking, DataLayers
	),

	NotBlueprintable,
	NotBlueprintType
)
class RE_API AReWater final : public AActor
{
	GENERATED_BODY()

public:

	AReWater();

private:

	virtual void OnConstruction(const FTransform& Transform) override;

private:

	UPROPERTY() TObjectPtr<UStaticMeshComponent> StaticMeshComponent;

	UPROPERTY
	(
		Category = "Properties", EditAnywhere,

		meta =
		(
			Delta = "1.0", ClampMin = "0.1", ClampMax = "10000.0", UIMax = "1000.0"
		)
	)
	float Scale = 10.0f;
};
