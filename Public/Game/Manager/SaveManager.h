#pragma once

#include "Misc/Guid.h"

class UReSaveGame;
class ULoadingScreen;

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SaveManager.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnGameSaved, UReSaveGame*);
DECLARE_MULTICAST_DELEGATE(FOnGameLoadStarted);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnGameLoaded, const UReSaveGame*);
DECLARE_MULTICAST_DELEGATE(FOnGameLoadComplete);

UCLASS() class RE_API USaveManager final : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:

	// Other
	
	bool IsGameLoading() const;
	void SaveGame();
	void StartGameLoad();
	void CompleteGameLoad();
	bool IsItemInstancePickedUp(const FGuid& InGuid) const;
	void MarkItemInstancePickedUp(const FGuid& InGuid);

private:

	// Delegate Listeners

	void OnFadeInCompleteListener();
	void OnFadeOutCompleteListener();

public:

	// Delegate Instances

	FOnGameSaved OnGameSavedDelegate;
	FOnGameLoadStarted OnGameLoadStartedDelegate;
	FOnGameLoaded OnGameLoadedDelegate;
	FOnGameLoadComplete OnGameLoadCompleteDelegate;

private:
	
	// Properties

	inline static constexpr TCHAR SaveFileName[] = TEXT("ReSaveFile");

	// Data
	
	UPROPERTY(Transient) TObjectPtr<UReSaveGame> LoadedSaveGame;
	UPROPERTY(Transient) TObjectPtr<ULoadingScreen> LoadingScreen;
	UPROPERTY(Transient) TSet<FGuid> PickedUpItemInstances;
};