#include "Game/ReGameMode.h"
#include "Player/RePlayerController.h"
#include "Player/PlayerPawn.h"

AReGameMode::AReGameMode()
{
	PlayerControllerClass = ARePlayerController::StaticClass();
	DefaultPawnClass = APlayerPawn::StaticClass();
}

void AReGameMode::BeginPlay()
{
	Super::BeginPlay();
	
#if	WITH_EDITOR
	
	SetFolderPath(FName(TEXT("[0] Game")));
	SetActorLabel(TEXT("Game Mode"));
	
#endif
}