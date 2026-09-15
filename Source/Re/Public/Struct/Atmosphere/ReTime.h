#pragma once

#include "HAL/Platform.h"
#include "Math/ReMath.h"
#include "Math/UnrealMathUtility.h"
#include "Math/UnrealPlatformMathSSE.h"
#include "Misc/DateTime.h"
#include "UObject/ObjectMacros.h"
#include "ReTime.generated.h"

USTRUCT() struct FReTime
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, meta = (ClampMin = "0", ClampMax = "23")) uint8 Hour	= 0;
	UPROPERTY(EditAnywhere, meta = (ClampMin = "0", ClampMax = "59")) uint8 Minute	= 0;
	UPROPERTY(EditAnywhere, meta = (ClampMin = "0", ClampMax = "59")) uint8 Second	= 0;

public:

	FReTime() = default;

	FReTime(int32 InHour, int32 InMinute, int32 InSecond)
	: Hour		(FMath::Clamp(InHour,	0, 23))
	, Minute	(FMath::Clamp(InMinute,	0, 59))
	, Second	(FMath::Clamp(InSecond,	0, 59))
	{ }

	explicit FReTime(int32 InHour): FReTime(GetTimeFromHour(InHour)) { }

	/* implicit */ FReTime(FDateTime DateTime)
	: Hour		(DateTime.GetHour	())
	, Minute	(DateTime.GetMinute	())
	, Second	(DateTime.GetSecond	())
	{ }

	bool operator == (FReTime Time) const
	{
		return Hour == Time.Hour && Minute == Time.Minute && Second == Time.Second;
	}

	bool operator != (FReTime Time) const
	{
		return !(*this == Time);
	}

	bool operator > (FReTime Time) const
	{
		return GetSeconds() > Time.GetSeconds();
	}
	
	bool operator < (FReTime Time) const
	{
		return GetSeconds() < Time.GetSeconds();
	}

	bool operator >= (FReTime Time) const
	{
		return GetSeconds() >= Time.GetSeconds();
	}

	bool operator <= (FReTime Time) const
	{
		return GetSeconds() <= Time.GetSeconds();
	}

	FReTime operator + (FReTime Time) const
	{
		return GetTimeFromSeconds
		(
			GetSeconds() + Time.GetSeconds()
		);
	}

	FReTime operator - (FReTime Time) const
	{
		return GetTimeFromSeconds
		(
			GetSeconds() - Time.GetSeconds()
		);
	}

	FReTime& operator += (FReTime Time)
	{
		return *this = *this + Time;
	}

	FReTime& operator -= (FReTime Time)
	{
		return *this = *this - Time;
	}

	FReTime& operator ++ ()
	{
		Re::Math::Wrap(++Hour, 0, 24);
		return *this;
	}

	FReTime operator ++ (int)
	{
		const FReTime Temp = *this;
		++(*this);

		return Temp;
	}

	FReTime& operator -- ()
	{
		Re::Math::Wrap(--Hour, 0, 24);
		return *this;
	}

	FReTime operator -- (int)
	{
		const FReTime Temp = *this;
		--(*this);

		return Temp;
	}

	int32 GetSeconds() const
	{
		return Hour * 3600 + Minute * 60 + Second;
	}

	static FReTime GetTimeFromHour(int32 InHour)
	{
		return FReTime(InHour, 0, 0);
	}

	static FReTime GetTimeFromSeconds(int32 Seconds)
	{
		Re::Math::Wrap(Seconds, 0, 86400);

		const int32 Hour	= Seconds / 3600;
		const int32 Minute	= (Seconds % 3600) / 60;
		const int32 Second	= Seconds % 60;

		return FReTime(Hour, Minute, Second);
	}

	void AdvanceByDeltaTime(int32 Factor, float DeltaTime)
	{
		check(Factor >= 0);
		*this = GetTimeFromSeconds(GetSeconds() + FMath::RoundToInt32(Factor * DeltaTime));
	}
};
