#pragma once

#include "HAL/Platform.h"
#include "Math/Rotator.h"
#include "Math/Vector.h"
#include "Math/UnrealMathUtility.h"

namespace Re::Math
{
	/**
	 * [Min, Max)
	 */
	[[nodiscard]] inline uint8 GetWrapped(uint8 Value, uint8 Min, uint8 Max)
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
	[[nodiscard]] inline int32 GetWrapped(int32 Value, int32 Min, int32 Max)
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

	[[nodiscard]] inline float GetSaturated(float Value)
	{
		return FMath::Clamp(Value, 0.0f, 1.0f);
	}

	inline void Saturate(float& OutValue)
	{
		OutValue = GetSaturated(OutValue);
	}

	/**
	 * Returns true if the interpolation happened; false otherwise.
	 */
	[[nodiscard]] inline bool VInterp(FVector& OutCurrent, const FVector& Target, float InterpSpeed, float DeltaTime)
	{
		if (InterpSpeed <= 0.0f || DeltaTime <= 0.0f || Target == OutCurrent)
		{
			return false;
		}

		const FVector Delta = Target - OutCurrent;

		if (Delta.IsNearlyZero())
		{
			OutCurrent = Target;
			return true;
		}

		const float Alpha = InterpSpeed * DeltaTime;

		if (Alpha >= 1.0f)
		{
			OutCurrent = Target;
			return true;
		}

		OutCurrent += Delta * Alpha;
		return true;
	}

	/**
	 * Returns true if the interpolation happened; false otherwise.
	 */
	[[nodiscard]] inline bool RInterp(FRotator& OutCurrent, const FRotator& Target, const float InterpSpeed, const float DeltaTime)
	{
		if (InterpSpeed <= 0.0f || DeltaTime <= 0.0f || Target == OutCurrent)
		{
			return false;
		}

		const FRotator Delta = (Target - OutCurrent).GetNormalized();

		if (Delta.IsNearlyZero())
		{
			OutCurrent = Target;
			return true;
		}

		const float Alpha = InterpSpeed * DeltaTime;

		if (Alpha >= 1.0f)
		{
			OutCurrent = Target;
			return true;
		}

		OutCurrent = (OutCurrent + Delta * Alpha).GetNormalized();
		return true;
	}
}
