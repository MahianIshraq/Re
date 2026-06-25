#pragma once

#include "InputActionValue.h"

class UEnhancedInputComponent;
class APawn;
class APlayerCamera;
class APlayerController;
class USceneComponent;
class UInputAction;

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerMovementComponent.generated.h"

/**
 * - Walkable slope angle <= 45.
 * - Unwalkable slope angle >= 75.
 * - Never use any slope angle near 60.
 * - Slopes shouldn't be tilted.
 * - Player doesn't slide down from unwalkable slopes, but can walk down.
 */

UCLASS
(
	HideCategories =
	(
		Tags,
		Activation,
		Cooking,
		AssetUserData,
		Navigation
	)
)
class RE_API UPlayerMovementComponent final : public UActorComponent
{
	GENERATED_BODY()

public:

	UPlayerMovementComponent();

	// Getters

	float GetEffectiveSpeed() const;
	bool GetIsGrounded() const;

	// Other

	void SetupInput(UEnhancedInputComponent* InEnhancedInputComponent);
	void Setup(const APawn* InPawn, const APlayerCamera* InPlayerCamera, float InHalfHeight, float InRadius);

private:

	virtual void TickComponent(float InDeltaTime, ELevelTick InTickType, FActorComponentTickFunction* InThisTickFunction) override;

	// IA | Overworld
	
	void StartMouseMovement();
	void UpdateMouseMovement();
	void CompleteMovement();
	void UpdateGamepadMovement(const FInputActionValue& InValue);

	// Helper Functions

	void Decelerate(float InDeltaTime);
	void RotateToMovement(float InDeltaTime);
	void Accelerate(float InDeltaTime);
	float SlideAlongSurface(const FVector& InDelta, const FVector& InSurfaceNormal, float InTime, int32 InMaxSurfaceCount = 4);
	static bool IsWalkableSurface(const FVector& InSurfaceNormal);
	static FVector TwoWallAdjust(const FVector& InCurrentSurfaceNormal, const FVector& InNextSurfaceNormal, const FVector& InCurrentSlideDelta, float InTime);
	
private:
	
	// Properties
	
	UPROPERTY(Category = "Properties", EditAnywhere, meta = (ClampMin = "500.0", ClampMax = "5000.0"))
	float Acceleration = 1000.0f;
	
	UPROPERTY(Category = "Properties", EditAnywhere, meta = (ClampMin = "500.0", ClampMax = "5000.0"))
	float Deceleration = 2000.0f;
	
	UPROPERTY(Category = "Properties", EditAnywhere, meta = (ClampMin = "200.0", ClampMax = "800.0"))
	float SpeedLimit = 400.0f;

	UPROPERTY(Category = "Properties", EditAnywhere, meta = (ClampMin = "500.0", ClampMax = "5000.0"))
	float GravitySpeed = 1200.0f;

	UPROPERTY(Category = "Properties", EditAnywhere, meta = (ClampMin = "10.0", ClampMax = "50.0"))
	float TurnRate = 25.0f;

	// Data

	UPROPERTY(Category = "Data", VisibleAnywhere, Transient)
	FVector2D MovementInput = FVector2D::ZeroVector;

	UPROPERTY(Category = "Data", VisibleAnywhere, Transient)
	float RawSpeed = 0.0f;

	UPROPERTY(Category = "Data", VisibleAnywhere, Transient)
	float EffectiveSpeed = 0.0f;

	UPROPERTY(Category = "Data", VisibleAnywhere, Transient)
	bool bIsGrounded = true;

	// Caches

	TWeakObjectPtr<USceneComponent> UpdatedComponentCache;
	TWeakObjectPtr<const APlayerController> PlayerControllerCache;
	TWeakObjectPtr<const APlayerCamera> PlayerCameraCache;
	float HalfHeightCache = 0.0f;
	float RadiusCache = 0.0f;
	
	// IA | Overworld

	UPROPERTY(Transient) TObjectPtr<const UInputAction> IA_MouseMove;
	UPROPERTY(Transient) TObjectPtr<UInputAction> IA_GamepadMove;
	
	// Helper Variables

	bool bBlockMouseMovement = false;
};