#pragma once

#include "GameFramework/InputSettings.h"
#include "InputActionValue.h"

class UEnhancedInputComponent;
class APlayerController;
class UReSaveGame;
class USceneComponent;
class UCameraComponent;
class UInputAction;

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlayerCamera.generated.h"

UCLASS
(
	HideCategories =
	(
		Rendering,
		HLOD,
		Replication,
		Collision,
		Physics,
		Networking,
		Input,
		Actor,
		WorldPartition,
		LevelInstance,
		Cooking,
		DataLayers
	)
)
class RE_API APlayerCamera final : public AActor
{
	GENERATED_BODY()
	
public:
	
	APlayerCamera();

	// Setup
	
	void SetupInput(UEnhancedInputComponent* InEnhancedInputComponent);
	void Setup(const APlayerController* InPlayerController);

private:
	
	virtual void BeginPlay() override;
	virtual void Tick(float InDeltaTime) override;

	// Delegate Listeners

	void OnGameSavedListener(UReSaveGame* InSaveGame);
	void OnGameLoadedListener(const UReSaveGame* InSaveGame);
	void OnInputDeviceChangedListener(EHardwareDevicePrimaryType InputDeviceType);
	
	// IA | Overworld

	void MousePanStarted();
	void MousePan();
	void MousePanCompleted();
	void GamepadPanStarted();
	void GamepadPan(const FInputActionValue& InValue);
	void GamepadPanCompleted();
	void CenterView();
	void LeftRotateViewPressed();
	void LeftRotateViewReleased();
	void RightRotateViewPressed();
	void RightRotateViewReleased();

	// Helper Functions

	void TickLocation(float InDeltaTime);
	void TickRotation(float InDeltaTime);
	void ApplyPan(const FVector2D& InPanDelta);
	
private:

	// Components
	
	UPROPERTY(VisibleDefaultsOnly) TObjectPtr<USceneComponent> PivotComponent;
	UPROPERTY(VisibleDefaultsOnly) TObjectPtr<UCameraComponent> CameraComponent;

	// Properties | Camera View

	inline static constexpr float OrthoWidth = 4000.0f;
	inline static constexpr float AutoPlaneShift = -3000.0f;
	inline static constexpr float AspectRatio = 16.0f / 9.0f;
	inline static constexpr float BasePitch = -52.0f;
	inline static constexpr float OcclusionPitch = -75.0f;
	inline static constexpr float BaseYaw = -45.0f;

	// Properties | Camera Rotation

	UPROPERTY(Category = "Properties | Camera Rotation", EditAnywhere)
	bool bInvertRotation = false;
	
	UPROPERTY(Category = "Properties | Camera Rotation", EditAnywhere)
	float RotationDelta = 15.0f;

	// Properties | Camera Lag

	UPROPERTY(Category = "Properties | Camera Lag", EditAnywhere)
	bool bEnableLocationLag = true;

	UPROPERTY(Category = "Properties | Camera Lag", EditAnywhere)
	float LocationLagSpeed = 5.0f;
	
	UPROPERTY(Category = "Properties | Camera Lag", EditAnywhere)
	bool bEnableRotationLag = true;
	
	UPROPERTY(Category = "Properties | Camera Lag", EditAnywhere)
	float RotationLagSpeed = 10.0f;

	// Properties | Other

	inline static constexpr float MaxPanDistance = 2000.0f;
	inline static constexpr float GamepadPanFactor = 5.0f;
	inline static constexpr float AutoCenterPaddingRatio = 0.1f;

	// Caches | Setup

	TWeakObjectPtr<const APlayerController> PlayerControllerCache;
	TWeakObjectPtr<const AActor> TargetActorCache;

	// Assets | IA | Overworld

	UPROPERTY(Transient) TObjectPtr<UInputAction> IA_GamepadPan;
	UPROPERTY(Transient) TObjectPtr<const UInputAction> IA_MousePan;
	UPROPERTY(Transient) TObjectPtr<const UInputAction> IA_CenterView;
	UPROPERTY(Transient) TObjectPtr<const UInputAction> IA_LeftRotateView;
	UPROPERTY(Transient) TObjectPtr<const UInputAction> IA_RightRotateView;

	// Helper Variables

	bool bIsMousePanning = false;
	bool bIsGamepadPanning = false;
	FVector2D LastMousePanPosition = FVector2D::ZeroVector;
	
	FVector TargetLocation = FVector::ZeroVector;
	FVector InterpolatedLocation = FVector::ZeroVector;
	FRotator TargetRotation = FRotator::ZeroRotator;
	FRotator InterpolatedRotation = FRotator::ZeroRotator;
};