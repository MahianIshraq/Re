#pragma once

#include "InputActionValue.h"
#include "Engine/HitResult.h"

class UEnhancedInputComponent;
class APawn;
class APlayerCamera;
class USceneComponent;
class APlayerController;
class UInputAction;

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerMovementComponent.generated.h"

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
	
	void UpdateMouseMovement();
	void UpdateGamepadMovement(const FInputActionValue& InValue);

	// Helper Functions

	void Decelerate(float InDeltaTime);
	void RotateToMovement(float InDeltaTime);
	void SlideAlongSurface(const FVector& InDelta, const FHitResult& InHitResult, float InTime, int32 InMaxSlideCount = 2);
	bool IsWalkableSurface(const FVector& InSurfaceNormal) const;
	static FVector TwoWallAdjust(const FVector& InCurrentSurfaceNormal, const FVector& InNextSurfaceNormal, const FVector& InCurrentSlideDelta, float InTime);
	
private:
	
	// Properties | Gravity

	UPROPERTY(Category = "Properties | Gravity", EditAnywhere, meta = (ClampMin = "500.0", ClampMax = "5000.0"))
	float GravitySpeed = 1960.0f;

	UPROPERTY(Category = "Properties | Gravity", EditAnywhere, meta = (ClampMin = "500.0", ClampMax = "5000.0"))
	float GravitySlideFactor = 0.25f;
	
	// Properties | Movement
	
	UPROPERTY(Category = "Properties | Movement", EditAnywhere, meta = (ClampMin = "500.0", ClampMax = "5000.0"))
	float Acceleration = 1000.0f;
	
	UPROPERTY(Category = "Properties | Movement", EditAnywhere, meta = (ClampMin = "500.0", ClampMax = "5000.0"))
	float Deceleration = 2000.0f;
	
	UPROPERTY(Category = "Properties | Movement", EditAnywhere, meta = (ClampMin = "200.0", ClampMax = "800.0"))
	float MaxSpeed = 400.0f;

	UPROPERTY(Category = "Properties | Movement", EditAnywhere, meta = (ClampMin = "10.0", ClampMax = "50.0"))
	float TurnRate = 25.0f;

	// Properties | Slope

	/**
	 * - MaxWalkableSlopeAngle is not accurate.
	 * - ActualMaxWalkableSlopeAngle ~= MaxWalkableSlopeAngle + 5.0f.
	 */
	UPROPERTY(Category = "Properties | Slope", EditAnywhere, meta = (ClampMin = "10.0", ClampMax = "75.0"))
	float MaxWalkableSlopeAngle = 45.0f;

	// Properties | Physics
	
	UPROPERTY(Category = "Properties | Physics", EditAnywhere, meta = (ClampMin = "0.0", ClampMax = "500.0"))
	float ImpulseFactor = 0.1f;
	
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

	bool bIsMovementPending = false;
};