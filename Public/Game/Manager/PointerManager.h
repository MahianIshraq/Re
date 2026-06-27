#pragma once

#include "Enum/Game/EPointer.h"
#include "Enum/Game/EContext.h"
#include "GameFramework/InputSettings.h"

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "PointerManager.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnPointerCaptured);

UCLASS() class RE_API UPointerManager final : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:

	// Setters

	void SetPointer(EPointer InPointer);

	// Other
	
	bool IsCaptured() const;
	void CapturePointer();
	void ReleasePointer();
	void ShowPointer();
	void HidePointer();

private:

	virtual void Initialize(FSubsystemCollectionBase& InCollection) override;

	// Delegate Listeners

	void OnContextChangedListener(EContext);
	void OnInputDeviceChangedListener(EHardwareDevicePrimaryType InputDeviceType);

	// Helper Functions

	void RefreshPointer();

public:

	// Delegate Instances

	FOnPointerCaptured OnPointerCapturedDelegate;

private:

	// Data

	EPointer Pointer = EPointer::None;
	EPointer CapturedPointer = EPointer::None;
};