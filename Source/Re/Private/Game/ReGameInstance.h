#pragma once

#include "Delegates/Delegate.h"
#include "Delegates/DelegateCombinations.h"
#include "Delegates/IDelegateInstance.h"
#include "Engine/GameInstance.h"
#include "ReGameInstance.generated.h"

class APlayerController;
class UEnhancedInputComponent;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnInputInitialized,	UEnhancedInputComponent*);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnGameInitialized,		APlayerController*);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnGameStarted,			APlayerController*);

UCLASS() class UReGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:

	virtual void	Init				() override;
	virtual void	Shutdown			() override;

	void			InitializeInput		(UEnhancedInputComponent*	EnhancedInputComponent);
	void			InitializeGame		(APlayerController*			PlayerController);
	void			StartGame			(APlayerController*			PlayerController);

	template <typename Class> FDelegateHandle AddOnInputInitialized
	(
		Class* Object, void (Class::* Function)(UEnhancedInputComponent*)
	)
	{
		return OnInputInitialized.AddUObject(Object, Function);
	}

	template <typename Class> FDelegateHandle AddOnGameInitialized
	(
		Class* Object, void (Class::* Function)(APlayerController*)
	)
	{
		return OnGameInitialized.AddUObject(Object, Function);
	}

	template <typename Class> FDelegateHandle AddOnGameStarted
	(
		Class* Object, void (Class::* Function)(APlayerController*)
	)
	{
		return OnGameStarted.AddUObject(Object, Function);
	}

	void RemoveOnInputInitialized	(FDelegateHandle Handle);
	void RemoveOnGameInitialized	(FDelegateHandle Handle);
	void RemoveOnGameStarted		(FDelegateHandle Handle);

private:

	FOnInputInitialized		OnInputInitialized;
	FOnGameInitialized		OnGameInitialized;
	FOnGameStarted			OnGameStarted;
};
