#include "Game/Manager/PointerManager.h"
#include "Struct/Asset/FTexturePaths.h"
#include "Engine/World.h"
#include "Engine/GameViewportClient.h"
#include "Game/ReLog.h"

void UPointerManager::SetPointer(EPointer InPointer)
{
	if (InPointer == EPointer::None)
	{
		return;
	}

	Pointer = InPointer;
	RefreshPointer();
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