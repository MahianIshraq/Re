// ReSharper disable CppMemberFunctionMayBeConst

#include "Game/ReGameInstance.h"

#include "Engine/Engine.h"
#include "System/ReLog.h"

void UReGameInstance::Init()
{
	Super::Init();

#if WITH_EDITOR

	UWorld* World = GetWorld();

	GEngine->Exec(World, TEXT("sg.ResolutionQuality 50"		));
	GEngine->Exec(World, TEXT("sg.ViewDistanceQuality 0"	));
	GEngine->Exec(World, TEXT("sg.AntiAliasingQuality 0"	));
	GEngine->Exec(World, TEXT("sg.PostProcessQuality 0"		));

#endif
}

void UReGameInstance::Shutdown()
{
	Super::Shutdown();
}

void UReGameInstance::InitializeInput(UEnhancedInputComponent* EnhancedInputComponent)
{
	OnInputInitialized.Broadcast(EnhancedInputComponent);
	RE_LOG("Input initialized.");
}

void UReGameInstance::InitializeGame(APlayerController* PlayerController)
{
	OnGameInitialized.Broadcast(PlayerController);
	RE_LOG("Game initialized.");
}

void UReGameInstance::StartGame(APlayerController* PlayerController)
{
	OnGameStarted.Broadcast(PlayerController);
	RE_LOG("Game started.");
}

void UReGameInstance::RemoveOnInputInitialized(FDelegateHandle Handle)
{
	OnInputInitialized.Remove(Handle);
}

void UReGameInstance::RemoveOnGameInitialized(FDelegateHandle Handle)
{
	OnGameInitialized.Remove(Handle);
}

void UReGameInstance::RemoveOnGameStarted(FDelegateHandle Handle)
{
	OnGameStarted.Remove(Handle);
}
