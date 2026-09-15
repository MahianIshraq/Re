#pragma once

#include "HAL/Platform.h"
#include "Math/UnrealMathUtility.h"

namespace Re::Math
{
	/**
	 * [Min, Max)
	 */
	inline uint8 GetWrapped(uint8 Value, uint8 Min, uint8 Max)
	{
		return (Value - Min) % (Max - Min) + Min;
	}

	/**
	 * [Min, Max)
	 */
	inline void Wrap(uint8& OutValue, uint8 Min, uint8 Max)
	{
		OutValue = GetWrapped(OutValue, Min, Max);
	}

	/**
	 * [Min, Max)
	 */
	inline int32 GetWrapped(int32 Value, int32 Min, int32 Max)
	{
		const int32 Range = Max - Min;
		return ((Value - Min) % Range + Range) % Range + Min;
	}

	/**
	 * [Min, Max)
	 */
	inline void Wrap(int32& OutValue, int32 Min, int32 Max)
	{
		OutValue = GetWrapped(OutValue, Min, Max);
	}

	inline float GetSaturated(float Value)
	{
		return FMath::Clamp(Value, 0.0f, 1.0f);
	}

	inline void Saturate(float& OutValue)
	{
		OutValue = GetSaturated(OutValue);
	}
}
