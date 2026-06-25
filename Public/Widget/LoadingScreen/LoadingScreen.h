#pragma once

class UReBorder;

#include "CoreMinimal.h"
#include "Widget/ReWidget.h"
#include "LoadingScreen.generated.h"

DECLARE_DELEGATE(FOnFadeInComplete);
DECLARE_DELEGATE(FOnFadeOutComplete);

struct FLoadingScreenParams
{
public:
	
	float FadeInDuration = 0.5f;
	float PauseDuration = 0.5f;
	float FadeOutDuration = 0.5f;

	FOnFadeInComplete OnFadeInCompleteDelegate;
	FOnFadeOutComplete OnFadeOutCompleteDelegate;

private:

	inline static constexpr float MinimumTotalDuration = 0.3f;
	inline static constexpr float MinimumPhaseDuration = MinimumTotalDuration / 3.0f;
	inline static constexpr float DefaultTotalDuration = 1.5f;
	inline static constexpr float DefaultPhaseDuration = DefaultTotalDuration / 3.0f;

public:

	FLoadingScreenParams() = default;
	
	explicit FLoadingScreenParams(float InTotalDuration)
	{
		InTotalDuration = InTotalDuration >= MinimumTotalDuration ? InTotalDuration : DefaultTotalDuration;
		FadeInDuration = PauseDuration = FadeOutDuration = InTotalDuration / 3.0f;
	}
	
	FLoadingScreenParams
	(
		float InFadeInDuration,
		float InPauseDuration,
		float InFadeOutDuration
	)
	{
		FadeInDuration = InFadeInDuration >= MinimumPhaseDuration ? InFadeInDuration : DefaultPhaseDuration;
		PauseDuration = InPauseDuration >= MinimumPhaseDuration ? InPauseDuration : DefaultPhaseDuration;
		FadeOutDuration = InFadeOutDuration >= MinimumPhaseDuration ? InFadeOutDuration : DefaultPhaseDuration;
	}

	float GetTotalDuration() const
	{
		return FadeInDuration + PauseDuration + FadeOutDuration;
	}
};

UCLASS() class RE_API ULoadingScreen final : public UReWidget
{
	GENERATED_BODY()

public:

	void StartLoadingScreen(const FLoadingScreenParams& InParams = FLoadingScreenParams());
	void PauseLoadingScreen();
	void ContinueLoadingScreen();

private:

	virtual void NativeOnInitialized() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:

	// Components

	UPROPERTY(Transient) TObjectPtr<UReBorder> Border;

	// Data
	
	FLoadingScreenParams Params;

	// Helper Variables

	bool bTick = false;
	float ElapsedTime = 0.0f;
};