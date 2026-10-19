#pragma once

#include "Delegates/IDelegateInstance.h"
#include "Engine/EngineTypes.h"
#include "Enum/Game/EReContext.h"
#include "GameFramework/PlayerController.h"
#include "Math/Rotator.h"
#include "Math/Vector.h"
#include "UObject/ObjectMacros.h"
#include "UObject/ObjectPtr.h"
#include "UObject/WeakObjectPtrTemplates.h"
#include "RePlayerController.generated.h"

struct	FMinimalViewInfo;
class	UEnhancedInputLocalPlayerSubsystem;
class	UReContextManager;
class	UReGameInstance;
class	URePlayerCameraComponent;
class	UInputAction;
class	UInputMappingContext;
class	UReInputManager;
class	UReSaveManager;

UCLASS
(
	HideCategories = (PlayerController, MouseInterface, Game, Input, WorldPartition, HLOD, Replication, Physics, Networking, Actor, LevelInstance, Cooking, DataLayers),
	NotBlueprintable, NotBlueprintType
)
class ARePlayerController final : public APlayerController
{
	GENERATED_BODY()

public:

					ARePlayerController						();

private:

	virtual void	PostInitializeComponents				() override;
	virtual void	BeginPlay								() override;
	virtual void	EndPlay									(EEndPlayReason::Type Reason) override;

	virtual void	CalcCamera								(float DeltaTime, FMinimalViewInfo& OutResult) override;
	virtual void	GetStreamingSourceLocationAndRotation	(FVector& OutLocation, FRotator& OutRotation) const override;

	void			SetActiveInputMappingContext			(const UInputMappingContext* NewIMC);

	void			OnGameInitialized						();
	void			OnGameStarted							(APlayerController*);
	void			OnContextApplied						(EReContext Context);
	void			OnInputEnabled							();
	void			OnInputDisabled							();

	void			EscapePressed							();
	void			EscapeReleased							();
	void			ToggleInventoryPressed					();
	void			ToggleInventoryReleased					();

	void			GamepadFocusUp							();
	void			GamepadFocusDown						();
	void			GamepadFocusLeft						();
	void			GamepadFocusRight						();

	void			Interact								();
	void			Inspect									();
	void			QuickSavePressed						();
	void			QuickSaveReleased						();
	void			QuickLoadPressed						();
	void			QuickLoadReleased						();

private:

	UPROPERTY(VisibleDefaultsOnly) TObjectPtr<URePlayerCameraComponent> CameraComponent;

	UPROPERTY(Category = "Data",					VisibleAnywhere, Transient) TObjectPtr<const UInputMappingContext>				ActiveIMC;
	UPROPERTY(Category = "Cache",					VisibleAnywhere, Transient) TWeakObjectPtr<UEnhancedInputLocalPlayerSubsystem>	EnhancedInputLocalPlayerSubsystem;
	UPROPERTY(Category = "Cache",					VisibleAnywhere, Transient) TWeakObjectPtr<UReGameInstance>						ReGameInstance;
	UPROPERTY(Category = "Cache",					VisibleAnywhere, Transient) TWeakObjectPtr<UReContextManager>					ContextManager;
	UPROPERTY(Category = "Cache",					VisibleAnywhere, Transient) TWeakObjectPtr<UReSaveManager>						SaveManager;
	UPROPERTY(Category = "Cache",					VisibleAnywhere, Transient) TWeakObjectPtr<UReInputManager>						InputManager;

	UPROPERTY(Category = "Assets | IMC",			VisibleAnywhere, Transient) TObjectPtr<const UInputMappingContext>				IMC_Universal;
	UPROPERTY(Category = "Assets | IMC",			VisibleAnywhere, Transient) TObjectPtr<const UInputMappingContext>				IMC_Menu;
	UPROPERTY(Category = "Assets | IMC",			VisibleAnywhere, Transient) TObjectPtr<const UInputMappingContext>				IMC_Overworld;

	UPROPERTY(Category = "Assets | IA | Universal",	VisibleAnywhere, Transient) TObjectPtr<UInputAction>							IA_Escape;
	UPROPERTY(Category = "Assets | IA | Universal",	VisibleAnywhere, Transient) TObjectPtr<UInputAction>							IA_ToggleInventory;

	UPROPERTY(Category = "Assets | IA | Menu",		VisibleAnywhere, Transient) TObjectPtr<UInputAction>							IA_GamepadFocusUp;
	UPROPERTY(Category = "Assets | IA | Menu",		VisibleAnywhere, Transient) TObjectPtr<UInputAction>							IA_GamepadFocusDown;
	UPROPERTY(Category = "Assets | IA | Menu",		VisibleAnywhere, Transient) TObjectPtr<UInputAction>							IA_GamepadFocusLeft;
	UPROPERTY(Category = "Assets | IA | Menu",		VisibleAnywhere, Transient) TObjectPtr<UInputAction>							IA_GamepadFocusRight;

	UPROPERTY(Category = "Assets | IA | Overworld",	VisibleAnywhere, Transient) TObjectPtr<const UInputAction>						IA_Interact;
	UPROPERTY(Category = "Assets | IA | Overworld",	VisibleAnywhere, Transient) TObjectPtr<const UInputAction>						IA_Inspect;
	UPROPERTY(Category = "Assets | IA | Overworld",	VisibleAnywhere, Transient) TObjectPtr<const UInputAction>						IA_QuickSave;
	UPROPERTY(Category = "Assets | IA | Overworld",	VisibleAnywhere, Transient) TObjectPtr<const UInputAction>						IA_QuickLoad;

	FDelegateHandle OnGameInitializedHandle;
	FDelegateHandle OnGameStartedHandle;
	FDelegateHandle OnContextAppliedHandle;
	FDelegateHandle OnInputEnabledHandle;
	FDelegateHandle OnInputDisabledHandle;
};
