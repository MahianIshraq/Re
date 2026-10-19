#pragma once

#include "Delegates/Delegate.h"
#include "Delegates/DelegateCombinations.h"
#include "Delegates/IDelegateInstance.h"
#include "Engine/GameInstance.h"
#include "HAL/Platform.h"
#include "UObject/ObjectMacros.h"
#include "UObject/WeakObjectPtrTemplates.h"
#include "ReGameInstance.generated.h"

class APlayerController;
class UEnhancedInputComponent;

DECLARE_MULTICAST_DELEGATE			(FOnGameInitialized);
DECLARE_MULTICAST_DELEGATE_OneParam	(FOnGameStarted, APlayerController*);
DECLARE_MULTICAST_DELEGATE			(FOnGameEnded);

UCLASS() class UReGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:

	virtual void							Init						() override;

	[[nodiscard]] bool						HasGameInitialized			() const;
	[[nodiscard]] bool						HasGameStarted				() const;
	[[nodiscard]] bool						HasGameEnded				() const;
	[[nodiscard]] APlayerController*		GetPlayerController			() const;
	[[nodiscard]] UEnhancedInputComponent*	GetEnhancedInputComponent	() const;

	void									InitializeGame				();
	void									StartGame					(APlayerController* InPlayerController);
	void									EndGame						();

	template <typename Class> FDelegateHandle AddOnGameInitialized
	(
		Class* Object, void (Class::* Function)()
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

	template <typename Class> FDelegateHandle AddOnGameEnded
	(
		Class* Object, void (Class::* Function)()
	)
	{
		return OnGameEnded.AddUObject(Object, Function);
	}

	void RemoveOnGameInitialized	(FDelegateHandle Handle);
	void RemoveOnGameStarted		(FDelegateHandle Handle);
	void RemoveOnGameEnded			(FDelegateHandle Handle);

private:

	UPROPERTY(Category = "Data",	VisibleAnywhere, Transient) uint8 									bHasGameInitialized	: 1 = false;
	UPROPERTY(Category = "Data",	VisibleAnywhere, Transient) uint8 									bHasGameStarted		: 1 = false;
	UPROPERTY(Category = "Data",	VisibleAnywhere, Transient) uint8 									bHasGameEnded		: 1 = false;
	UPROPERTY(Category = "Cache",	VisibleAnywhere, Transient) TWeakObjectPtr<APlayerController>		PlayerController;
	UPROPERTY(Category = "Cache",	VisibleAnywhere, Transient) TWeakObjectPtr<UEnhancedInputComponent>	EnhancedInputComponent;

	FOnGameInitialized	OnGameInitialized;
	FOnGameStarted		OnGameStarted;
	FOnGameEnded		OnGameEnded;
};
