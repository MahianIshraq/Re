#pragma once

#include "GameFramework/InputSettings.h"

class UInputAction;
class APlayerController;

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "InputManager.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnInputEnabled);
DECLARE_MULTICAST_DELEGATE(FOnInputDisabled);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnInputDeviceChanged, EHardwareDevicePrimaryType);

UCLASS() class RE_API UInputManager final : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:

	// Getters

	EHardwareDevicePrimaryType GetInputDeviceType() const;
	
	// Setters
	
	void SetActiveClickableInputAction(const UInputAction* InInputAction);
	
	// Other

	void Setup(const APlayerController* PlayerController);
	void EnableInput();
	void DisableInput();
	bool TryConsumeActiveClickableInputAction(const UInputAction* InInputAction);
	
private:

	virtual void Initialize(FSubsystemCollectionBase& InCollection) override;

	// Delegate Listeners

	void OnInputHardwareDeviceChangedListener(FPlatformUserId, FInputDeviceId InDeviceID);

	// Helper Functions

	void RefreshInputDeviceType(FHardwareDeviceIdentifier InInputDevice);

public:

	// Delegate Instances

	FOnInputEnabled OnInputEnabledDelegate;
	FOnInputDisabled OnInputDisabledDelegate;
	FOnInputDeviceChanged OnInputDeviceChangedDelegate;

private:

	// Data

	EHardwareDevicePrimaryType InputDeviceType = EHardwareDevicePrimaryType::Unspecified;
	TWeakObjectPtr<const UInputAction> ActiveClickableInputAction;
};