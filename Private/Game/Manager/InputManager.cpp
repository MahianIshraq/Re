#include "Game/Manager/InputManager.h"
#include "InputAction.h"
#include "Engine/Engine.h"
#include "GameFramework/InputDeviceSubsystem.h"
#include "GameFramework/PlayerController.h"
#include "Game/Manager/ContextManager.h"
#include "Engine/GameInstance.h"
#include "Game/ReLog.h"

EHardwareDevicePrimaryType UInputManager::GetInputDeviceType() const
{
	return InputDeviceType;
}

void UInputManager::SetActiveClickableInputAction(const UInputAction* InInputAction)
{
	ActiveClickableInputAction = InInputAction;
}

void UInputManager::Setup(const APlayerController* PlayerController)
{
	check(PlayerController);
	
	RefreshInputDeviceType
	(
		GEngine->GetEngineSubsystem<UInputDeviceSubsystem>()->GetMostRecentlyUsedHardwareDevice(PlayerController->GetPlatformUserId())
	);
	
	RE_LOG("InputManager | Setup complete.");
}

void UInputManager::EnableInput()
{
	OnInputEnabledDelegate.Broadcast();
	RE_LOG("Enabled input.");
}

void UInputManager::DisableInput()
{
	OnInputDisabledDelegate.Broadcast();
	RE_LOG("Disabled input.");
}

bool UInputManager::TryConsumeActiveClickableInputAction(const UInputAction* InInputAction)
{
	if (InInputAction == ActiveClickableInputAction)
	{
		ActiveClickableInputAction.Reset();
		return true;
	}
	
	return false;
}

void UInputManager::Initialize(FSubsystemCollectionBase& InCollection)
{
	Super::Initialize(InCollection);

	// Dependencies
	
	InCollection.InitializeDependency(UContextManager::StaticClass());

	// Delegate Bindings

	GEngine->GetEngineSubsystem<UInputDeviceSubsystem>()->OnInputHardwareDeviceChangedNative.AddUObject(this, &UInputManager::OnInputHardwareDeviceChangedListener);
	GetGameInstance()->GetSubsystem<UContextManager>()->OnContextChangedDelegate.AddUObject(this, &UInputManager::OnContextChangedListener);
}

void UInputManager::OnInputHardwareDeviceChangedListener(FPlatformUserId, FInputDeviceId InDeviceID)
{
	RefreshInputDeviceType
	(
		GEngine->GetEngineSubsystem<UInputDeviceSubsystem>()->GetInputDeviceHardwareIdentifier(InDeviceID)
	);
}

void UInputManager::OnContextChangedListener(EContext)
{
	ActiveClickableInputAction.Reset();
}

void UInputManager::RefreshInputDeviceType(FHardwareDeviceIdentifier InInputDevice)
{
	if (InInputDevice.IsValid())
	{
		const EHardwareDevicePrimaryType NewDeviceType = InInputDevice.PrimaryDeviceType;
		
		if (NewDeviceType == InputDeviceType)
		{
			return;
		}
		
		RE_LOG
		(
			"Input device type changed from {0} to {1}.",
			UEnum::GetValueAsString(InputDeviceType),
			UEnum::GetValueAsString(NewDeviceType)
		);
		
		InputDeviceType = NewDeviceType;
		OnInputDeviceChangedDelegate.Broadcast(InputDeviceType);
	}
	
	else
	{
		RE_WARN("Invalid input device.");
		InputDeviceType = EHardwareDevicePrimaryType::Unspecified;
	}
}