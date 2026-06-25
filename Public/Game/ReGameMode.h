#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ReGameMode.generated.h"

UCLASS
(
	HideCategories =
	(
		GameMode,
		Game,
		Physics,
		Networking,
		LevelInstance,
		Cooking
	)
)
class RE_API AReGameMode final : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	
	AReGameMode();

private:
	
	virtual void BeginPlay() override;
};