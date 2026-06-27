#include "Game/Manager/PointerManager.h"

#include "Engine/GameInstance.h"
#include "Game/Manager/ContextManager.h"
#include "Game/Manager/InputManager.h"
#include "Struct/Asset/FTexturePaths.h"
#include "Engine/World.h"
#include "Engine/GameViewportClient.h"
#include "Game/ReLog.h"

void UPointerManager::SetPointer(EPointer InPointer)
{
	if (CapturedPointer != EPointer::None)
	{
		return;
	}
	
	if (InPointer == Pointer || InPointer == EPointer::None)
	{
		return;
	}

	Pointer = InPointer;
	RefreshPointer();
}

bool UPointerManager::IsCaptured() const
{
	return CapturedPointer != EPointer::None;
}

void UPointerManager::ShowPointer()
{
	Pointer = EPointer::Default;
	RefreshPointer();
}

void UPointerManager::HidePointer()
{
	SetPointer(EPointer::Invisible);
}

void UPointerManager::Initialize(FSubsystemCollectionBase& InCollection)
{
	Super::Initialize(InCollection);
	
	InCollection.InitializeDependency(UContextManager::StaticClass());
	InCollection.InitializeDependency(UInputManager::StaticClass());
	
	GetGameInstance()->GetSubsystem<UContextManager>()->OnContextChangedDelegate.AddUObject(this, &UPointerManager::OnContextChangedListener);
	GetGameInstance()->GetSubsystem<UInputManager>()->OnInputDeviceChangedDelegate.AddUObject(this, &UPointerManager::OnInputDeviceChangedListener);
}

void UPointerManager::OnContextChangedListener(EContext)
{
	ReleasePointer();
}

void UPointerManager::OnInputDeviceChangedListener(EHardwareDevicePrimaryType InputDeviceType)
{
	ReleasePointer();

	if (InputDeviceType == EHardwareDevicePrimaryType::KeyboardAndMouse)
	{
		ShowPointer();
	}
	
	else if (InputDeviceType == EHardwareDevicePrimaryType::Gamepad)
	{
		HidePointer();
	}
}

void UPointerManager::CapturePointer()
{
	CapturedPointer = Pointer;
	RE_LOG("Captured {0}.", UEnum::GetValueAsString(CapturedPointer));
}

void UPointerManager::ReleasePointer()
{
	RE_LOG("Released {0}.", UEnum::GetValueAsString(CapturedPointer));
	CapturedPointer = EPointer::None;
	SetPointer(EPointer::Default);
}

void UPointerManager::RefreshPointer()
{
	FName Path;

	switch (Pointer)
	{
		case EPointer::Invisible:
		{
			Path = FTexturePaths::T_InvisiblePointer;
			break;
		}
		
		case EPointer::Default:
		{
			Path = FTexturePaths::T_DefaultPointer;
			break;
		}

		case EPointer::Click:
		{
			Path = FTexturePaths::T_ClickPointer;
			break;
		}

		case EPointer::Grab:
		{
			Path = FTexturePaths::T_GrabPointer;
			break;
		}

		case EPointer::Pickup:
		{
			Path = FTexturePaths::T_PickupPointer;
			break;
		}

		case EPointer::Speak:
		{
			Path = FTexturePaths::T_SpeakPointer;
			break;
		}

		default:
		{
			RE_WARN("PointerManager | Illegal attempt to set Pointer to {0}.", UEnum::GetValueAsString(Pointer));
			return;
		}
	}
	
	GetWorld()->GetGameViewport()->SetHardwareCursor(EMouseCursor::Custom, Path, FVector2D(0.5f));
	RE_LOG("Pointer set to {0}.", UEnum::GetValueAsString(Pointer));
}
