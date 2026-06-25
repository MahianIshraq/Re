#pragma once

#include "CoreMinimal.h"

struct FTexturePaths
{
	// Pointer
	
	inline static constexpr TCHAR T_InvisiblePointer[] = TEXT("Root/Assets/Texture/UI/Pointer/T_InvisiblePointer");
	inline static constexpr TCHAR T_DefaultPointer[] = TEXT("Root/Assets/Texture/UI/Pointer/T_DefaultPointer");
	inline static constexpr TCHAR T_ClickPointer[] = TEXT("Root/Assets/Texture/UI/Pointer/T_ClickPointer");
	inline static constexpr TCHAR T_GrabPointer[] = TEXT("Root/Assets/Texture/UI/Pointer/T_GrabPointer");
	inline static constexpr TCHAR T_PickupPointer[] = TEXT("Root/Assets/Texture/UI/Pointer/T_PickupPointer");
	inline static constexpr TCHAR T_SpeakPointer[] = TEXT("Root/Assets/Texture/UI/Pointer/T_SpeakPointer");
	
	// Border

	inline static constexpr TCHAR T_Border_Outer[] = TEXT("/Game/Root/Assets/Texture/UI/Border/T_Border_Outer.T_Border_Outer");
	inline static constexpr TCHAR T_Border_Inner[] = TEXT("/Game/Root/Assets/Texture/UI/Border/T_Border_Inner.T_Border_Inner");
	inline static constexpr TCHAR T_ButtonBorder[] = TEXT("/Game/Root/Assets/Texture/UI/Border/T_ButtonBorder.T_ButtonBorder");
	inline static constexpr TCHAR T_ButtonBorder_Pressed[] = TEXT("/Game/Root/Assets/Texture/UI/Border/T_ButtonBorder_Pressed.T_ButtonBorder_Pressed");
};