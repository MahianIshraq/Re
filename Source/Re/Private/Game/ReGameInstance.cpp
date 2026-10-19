// ReSharper disable CppMemberFunctionMayBeConst

#include "Game/ReGameInstance.h"
#include "EnhancedInputComponent.h"
#include "RHIGlobals.h"
#include "GameFramework/Actor.h"
#include "GameFramework/PlayerController.h"
#include "HAL/IConsoleManager.h"
#include "Misc/AssertionMacros.h"
#include "System/ReLog.h"
#include "Templates/Casts.h"

void UReGameInstance::Init()
{
	Super::Init();

#if !WITH_EDITOR

	const IConsoleManager& ConsoleManager = IConsoleManager::Get();

	if (!GRHIGlobals.RayTracing.Supported)
	{
		ConsoleManager.FindConsoleVariable	(TEXT("r.ReflectionMethod"					))->Set(0);
		ConsoleManager.FindConsoleVariable	(TEXT("r.ShadowQuality"						))->Set(0);
	}

#endif
}

bool UReGameInstance::HasGameInitialized() const
{
	return bHasGameInitialized;
}

bool UReGameInstance::HasGameStarted() const
{
	return bHasGameStarted;
}

bool UReGameInstance::HasGameEnded() const
{
	return bHasGameEnded;
}

APlayerController* UReGameInstance::GetPlayerController() const
{
	return PlayerController.Get();
}

UEnhancedInputComponent* UReGameInstance::GetEnhancedInputComponent() const
{
	return EnhancedInputComponent.Get();
}

void UReGameInstance::InitializeGame()
{
	if (bHasGameInitialized)
	{
		return;
	}

	bHasGameInitialized	= true;
	bHasGameEnded		= false;

	OnGameInitialized.Broadcast();
	RE_LOG("Game initialized.");
}

void UReGameInstance::StartGame(APlayerController* InPlayerController)
{
	if (!bHasGameInitialized || bHasGameStarted)
	{
		return;
	}

	bHasGameStarted = true;

	PlayerController = InPlayerController;
	check(InPlayerController);

	EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerController->InputComponent);
	check(EnhancedInputComponent.Get());

	OnGameStarted.Broadcast(InPlayerController);
	RE_LOG("Game started.");
}

void UReGameInstance::EndGame()
{
	if (bHasGameEnded)
	{
		return;
	}

	bHasGameEnded			= true;
	bHasGameInitialized		= false;
	bHasGameStarted			= false;
	PlayerController		.Reset();
	EnhancedInputComponent	.Reset();

	OnGameEnded.Broadcast();
	RE_LOG("Game ended.");
}

void UReGameInstance::RemoveOnGameInitialized(FDelegateHandle Handle)
{
	OnGameInitialized.Remove(Handle);
}

void UReGameInstance::RemoveOnGameStarted(FDelegateHandle Handle)
{
	OnGameStarted.Remove(Handle);
}

void UReGameInstance::RemoveOnGameEnded(FDelegateHandle Handle)
{
	OnGameEnded.Remove(Handle);
}
