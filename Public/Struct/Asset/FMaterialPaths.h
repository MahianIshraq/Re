#pragma once

#include "CoreMinimal.h"

struct FMaterialPaths
{
	// Player Camera

	inline static constexpr TCHAR M_Tonemapper_PP[] = TEXT("/Game/Root/Assets/Material/PostProcess/Tonemapper/M_Tonemapper_PP.M_Tonemapper_PP");
	inline static constexpr TCHAR M_DepthFog_PP[] = TEXT("/Game/Root/Assets/Material/PostProcess/DepthFog/M_DepthFog_PP.M_DepthFog_PP");
	inline static constexpr TCHAR M_Pixelizer_PP[] = TEXT("/Game/Root/Assets/Material/PostProcess/Pixelizer/M_Pixelizer_PP.M_Pixelizer_PP");
	inline static constexpr TCHAR M_DitheredLighting_PP[] = TEXT("/Game/Root/Assets/Material/PostProcess/DitheredLighting/M_DitheredLighting_PP.M_DitheredLighting_PP");

	// Interactable
	
	inline static constexpr TCHAR M_Highlight_Overlay[] = TEXT("/Game/Root/Assets/Material/Surface/Overlay/M_Highlight_Overlay.M_Highlight_Overlay");
};