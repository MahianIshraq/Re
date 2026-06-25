#include "Widget/LoadingScreen/LoadingScreen.h"
#include "Engine/GameInstance.h"
#include "Game/Manager/ContextManager.h"
#include "Blueprint/WidgetTree.h"
#include "Widget/Border/ReBorder.h"

void ULoadingScreen::StartLoadingScreen(const FLoadingScreenParams& InParams)
{
	Params = InParams;

	bTick = true;
	ElapsedTime = 0.0f;
	AddToViewport(100);
	
	GetGameInstance()->GetSubsystem<UContextManager>()->SetContext(EContext::LoadingScreen);
}

void ULoadingScreen::PauseLoadingScreen()
{
	bTick = false;
}

void ULoadingScreen::ContinueLoadingScreen()
{
	bTick = true;
	AddToViewport(100);
}

void ULoadingScreen::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	// Construct

	Border = WidgetTree->ConstructWidget<UReBorder>();

	// Tree

	WidgetTree->RootWidget = Border;

	// This

	SetPointer(EPointer::Invisible);
}

void ULoadingScreen::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!bTick)
	{
		return;
	}

	if (ElapsedTime >= Params.GetTotalDuration())
	{
		Params.OnFadeOutCompleteDelegate.ExecuteIfBound();
		RemoveFromParent();
		return;
	}
	
	ElapsedTime += InDeltaTime;
	float Alpha = 0.0f;
	
	const float FadeInDuration = Params.FadeInDuration;
	const float PauseDuration = Params.PauseDuration;
	const float FadeOutDuration = Params.FadeOutDuration;

	if (ElapsedTime < FadeInDuration)
	{
		Alpha = ElapsedTime / FadeInDuration;
	}
	
	else if (ElapsedTime < FadeInDuration + PauseDuration)
	{
		Alpha = 1.0f;
	}
	
	else
	{
		Alpha = 1.0f - (ElapsedTime - FadeInDuration - PauseDuration) / FadeOutDuration;
	}

	Border->SetBrushColor(FLinearColor(0.0f, 0.0f, 0.0f, Alpha));
	
	if
	(
		FOnFadeInComplete& OnFadeInCompleteDelegate = Params.OnFadeInCompleteDelegate;
		
		ElapsedTime >= FadeInDuration
		&&
		OnFadeInCompleteDelegate.IsBound()
	)
	{
		OnFadeInCompleteDelegate.Execute();
		OnFadeInCompleteDelegate.Unbind();
	}
}