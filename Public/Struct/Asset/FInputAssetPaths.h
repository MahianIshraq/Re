#pragma once

#include "CoreMinimal.h"

struct FInputAssetPaths
{
	//// IMC
	
	inline static constexpr TCHAR IMC_Universal[] = TEXT("/Game/Root/Input/IMC_Universal.IMC_Universal");
	inline static constexpr TCHAR IMC_Menu[] = TEXT("/Game/Root/Input/IMC_Menu.IMC_Menu");
	inline static constexpr TCHAR IMC_Overworld[] = TEXT("/Game/Root/Input/IMC_Overworld.IMC_Overworld");

	//// IA | Universal
	
	inline static constexpr TCHAR IA_Escape[] = TEXT("/Game/Root/Input/Universal/IA_Escape.IA_Escape");
	inline static constexpr TCHAR IA_Back[] = TEXT("/Game/Root/Input/Universal/IA_Back.IA_Back");
	inline static constexpr TCHAR IA_ToggleInventory[] = TEXT("/Game/Root/Input/Universal/IA_ToggleInventory.IA_ToggleInventory");

	//// IA | Menu
	
	inline static constexpr TCHAR IA_GamepadFocusUp[] = TEXT("/Game/Root/Input/Menu/IA_GamepadFocusUp.IA_GamepadFocusUp");
	inline static constexpr TCHAR IA_GamepadFocusDown[] = TEXT("/Game/Root/Input/Menu/IA_GamepadFocusDown.IA_GamepadFocusDown");
	inline static constexpr TCHAR IA_GamepadFocusLeft[] = TEXT("/Game/Root/Input/Menu/IA_GamepadFocusLeft.IA_GamepadFocusLeft");
	inline static constexpr TCHAR IA_GamepadFocusRight[] = TEXT("/Game/Root/Input/Menu/IA_GamepadFocusRight.IA_GamepadFocusRight");

	//// IA | Overworld

	inline static constexpr TCHAR IA_Interact[] = TEXT("/Game/Root/Input/Overworld/IA_Interact.IA_Interact");
	inline static constexpr TCHAR IA_Inspect[] = TEXT("/Game/Root/Input/Overworld/IA_Inspect.IA_Inspect");
	inline static constexpr TCHAR IA_QuickSave[] = TEXT("/Game/Root/Input/Overworld/IA_QuickSave.IA_QuickSave");
	inline static constexpr TCHAR IA_QuickLoad[] = TEXT("/Game/Root/Input/Overworld/IA_QuickLoad.IA_QuickLoad");
	
	// Player Movement Component

	inline static constexpr TCHAR IA_MouseMove[] = TEXT("/Game/Root/Input/Overworld/PlayerMovementComponent/IA_MouseMove.IA_MouseMove");
	inline static constexpr TCHAR IA_GamepadMove[] = TEXT("/Game/Root/Input/Overworld/PlayerMovementComponent/IA_GamepadMove.IA_GamepadMove");

	// Player Camera

	inline static constexpr TCHAR IA_MousePan[] = TEXT("/Game/Root/Input/Overworld/PlayerCamera/IA_MousePan.IA_MousePan");
	inline static constexpr TCHAR IA_GamepadPan[] = TEXT("/Game/Root/Input/Overworld/PlayerCamera/IA_GamepadPan.IA_GamepadPan");
	inline static constexpr TCHAR IA_CenterView[] = TEXT("/Game/Root/Input/Overworld/PlayerCamera/IA_CenterView.IA_CenterView");
	inline static constexpr TCHAR IA_LeftRotateView[] = TEXT("/Game/Root/Input/Overworld/PlayerCamera/IA_LeftRotateView.IA_LeftRotateView");
	inline static constexpr TCHAR IA_RightRotateView[] = TEXT("/Game/Root/Input/Overworld/PlayerCamera/IA_RightRotateView.IA_RightRotateView");
};