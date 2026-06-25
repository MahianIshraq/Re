#include "Game/Manager/SaveManager.h"
#include "Game/ReSaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "MoviePlayer.h"
#include "Widgets/Layout/SBorder.h"
#include "Widget/LoadingScreen/LoadingScreen.h"
#include "Game/ReLog.h"

bool USaveManager::IsGameLoading() const
{
	return LoadedSaveGame != nullptr;
}

void USaveManager::SaveGame()
{
	UReSaveGame* SaveGame = CastChecked<UReSaveGame>
	(
		UGameplayStatics::CreateSaveGameObject(UReSaveGame::StaticClass())
	);

	SaveGame->PickedUpItemInstances = PickedUpItemInstances;
	OnGameSavedDelegate.Broadcast(SaveGame);
	
	UGameplayStatics::AsyncSaveGameToSlot(SaveGame, SaveFileName, 0);
	RE_LOG("Game saved to {0}.sav.", SaveFileName);
}

void USaveManager::StartGameLoad()
{
	LoadedSaveGame = Cast<UReSaveGame>
	(
		UGameplayStatics::LoadGameFromSlot(SaveFileName, 0)
	);

	// Null LoadedSaveGame is legal.
	
	if (!LoadedSaveGame)
	{
		RE_WARN("No save file exists for game load.");
		return;
	}
	
	// Load PickedUpItemInstances early before level reload to allow Item Instances to destroy themselves.

	PickedUpItemInstances = LoadedSaveGame->PickedUpItemInstances;
	OnGameLoadStartedDelegate.Broadcast();
	RE_LOG("Game load started from {0}.sav.", SaveFileName);

	// MoviePlayer loading screen hides the ugly transition frame.

	FLoadingScreenAttributes LoadingScreenAttributes;
	LoadingScreenAttributes.WidgetLoadingScreen =
	(
		SNew(SBorder)
		.BorderImage(FCoreStyle::Get().GetBrush(TEXT("WhiteBrush")))
		.BorderBackgroundColor(FLinearColor::Black)
	);
	GetMoviePlayer()->SetupLoadingScreen(LoadingScreenAttributes);

	// Fade to black and opposite with ULoadingScreen widget.

	LoadingScreen = CreateWidget<ULoadingScreen>(GetGameInstance());
	
	FLoadingScreenParams LoadingScreenParams;
	LoadingScreenParams.OnFadeInCompleteDelegate.BindUObject(this, &USaveManager::OnFadeInCompleteListener);
	LoadingScreenParams.OnFadeOutCompleteDelegate.BindUObject(this, &USaveManager::OnFadeOutCompleteListener);
	
	LoadingScreen->StartLoadingScreen(LoadingScreenParams);
}

void USaveManager::CompleteGameLoad()
{
	OnGameLoadedDelegate.Broadcast(LoadedSaveGame);
	RE_LOG("Game loaded.");
	
	LoadedSaveGame = nullptr;
	LoadingScreen->ContinueLoadingScreen();
}

bool USaveManager::IsItemInstancePickedUp(const FGuid& InGuid) const
{
	return PickedUpItemInstances.Contains(InGuid);
}

void USaveManager::MarkItemInstancePickedUp(const FGuid& InGuid)
{
	check(InGuid.IsValid());
	PickedUpItemInstances.Add(InGuid);
}

void USaveManager::OnFadeInCompleteListener()
{
	LoadingScreen->PauseLoadingScreen();
	UGameplayStatics::OpenLevel(this, FName(UGameplayStatics::GetCurrentLevelName(this)));
	RE_LOG("Opening {0}.", UGameplayStatics::GetCurrentLevelName(this));
}

void USaveManager::OnFadeOutCompleteListener()
{
	OnGameLoadCompleteDelegate.Broadcast();
	RE_LOG("Game load complete.");

	LoadingScreen = nullptr;
}