#pragma once

#include "Enum/Game/EContext.h"

class UInputMappingContext;
class UInputAction;

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "RePlayerController.generated.h"

UCLASS
(
	HideCategories =
	(
		PlayerController,
		MouseInterface,
		Game,
		Input,
		WorldPartition,
		HLOD,
		Replication,
		Physics,
		Networking,
		Actor,
		LevelInstance,
		Cooking,
		DataLayers
	)
)
class RE_API ARePlayerController final : public APlayerController
{
	GENERATED_BODY()

public:

	ARePlayerController();
	
private:

	virtual void SetupInputComponent() override;
	virtual void BeginPlay() override;
	virtual void Tick(float InDeltaTime) override;

	// Delegate Listeners

	void OnContextChangedListener(EContext InContext);
	void OnGameLoadStartedListener();
	void OnInputEnabledListener();
	void OnInputDisabledListener();

	// IA | Universal

	void EscapePressed();
	void EscapeReleased();
	void ToggleInventoryPressed();
	void ToggleInventoryReleased();

	// IA | Menu

	void GamepadFocusUp();
	void GamepadFocusDown();
	void GamepadFocusLeft();
	void GamepadFocusRight();

	// IA | Overworld
	
	void Interact();
	void Inspect();
	void QuickSavePressed();
	void QuickSaveReleased();
	void QuickLoadPressed();
	void QuickLoadReleased();

	// Helper Functions
	
	void PreInit();
	void Init();
	void SetActiveInputMappingContext(const UInputMappingContext* InInputMappingContext);
	
private:

	// Data
	
	UPROPERTY(Transient) TObjectPtr<const UInputMappingContext> ActiveInputMappingContext;

	// Assets | IMC

	UPROPERTY(Transient) TObjectPtr<const UInputMappingContext> IMC_Universal;
	UPROPERTY(Transient) TObjectPtr<const UInputMappingContext> IMC_Menu;
	UPROPERTY(Transient) TObjectPtr<const UInputMappingContext> IMC_Overworld;

	// Assets | IA | Universal

	UPROPERTY(Transient) TObjectPtr<UInputAction> IA_Escape;
	UPROPERTY(Transient) TObjectPtr<UInputAction> IA_ToggleInventory;

	// Assets | IA | Menu

	UPROPERTY(Transient) TObjectPtr<UInputAction> IA_GamepadFocusUp;
	UPROPERTY(Transient) TObjectPtr<UInputAction> IA_GamepadFocusDown;
	UPROPERTY(Transient) TObjectPtr<UInputAction> IA_GamepadFocusLeft;
	UPROPERTY(Transient) TObjectPtr<UInputAction> IA_GamepadFocusRight;

	// Assets | IA | Overworld

	UPROPERTY(Transient) TObjectPtr<const UInputAction> IA_Interact;
	UPROPERTY(Transient) TObjectPtr<const UInputAction> IA_Inspect;
	UPROPERTY(Transient) TObjectPtr<const UInputAction> IA_QuickSave;
	UPROPERTY(Transient) TObjectPtr<const UInputAction> IA_QuickLoad;
};