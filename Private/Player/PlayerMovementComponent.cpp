#include "Player/PlayerMovementComponent.h"

#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "Components/SceneComponent.h"
#include "InputAction.h"
#include "UObject/ConstructorHelpers.h"
#include "Struct/Asset/FInputAssetPaths.h"
#include "EnhancedInputComponent.h"
#include "Engine/World.h"
#include "Engine/GameInstance.h"
#include "Game/Manager/InputManager.h"
#include "Struct/Shared/FTraceChannels.h"

#include "Player/PlayerCamera.h"

#include "Game/ReLog.h"

UPlayerMovementComponent::UPlayerMovementComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	
	IA_MouseMove = ConstructorHelpers::FObjectFinder<UInputAction>(FInputAssetPaths::IA_MouseMove).Object;
	IA_GamepadMove = ConstructorHelpers::FObjectFinder<UInputAction>(FInputAssetPaths::IA_GamepadMove).Object;
	
	IA_GamepadMove->ValueType = EInputActionValueType::Axis2D;
}

float UPlayerMovementComponent::GetEffectiveSpeed() const
{
	return EffectiveSpeed;
}

bool UPlayerMovementComponent::GetIsGrounded() const
{
	return bIsGrounded;
}

void UPlayerMovementComponent::SetupInput(UEnhancedInputComponent* InEnhancedInputComponent)
{
	check(InEnhancedInputComponent);
	InEnhancedInputComponent->BindAction(IA_MouseMove, ETriggerEvent::Started, this, &UPlayerMovementComponent::StartMouseMovement);
	InEnhancedInputComponent->BindAction(IA_MouseMove, ETriggerEvent::Triggered, this, &UPlayerMovementComponent::UpdateMouseMovement);
	InEnhancedInputComponent->BindAction(IA_MouseMove, ETriggerEvent::Completed, this, &UPlayerMovementComponent::CompleteMovement);
	InEnhancedInputComponent->BindAction(IA_GamepadMove, ETriggerEvent::Triggered, this, &UPlayerMovementComponent::UpdateGamepadMovement);
	InEnhancedInputComponent->BindAction(IA_GamepadMove, ETriggerEvent::Completed, this, &UPlayerMovementComponent::CompleteMovement);
}

void UPlayerMovementComponent::Setup(const APawn* InPawn, const APlayerCamera* InPlayerCamera, float InHalfHeight, float InRadius)
{
	check(InPawn && InPlayerCamera && InHalfHeight > 0.0f && InRadius > 0.0f);
	
	UpdatedComponentCache = InPawn->GetRootComponent();
	PlayerControllerCache = CastChecked<APlayerController>(InPawn->GetController());
	PlayerCameraCache = InPlayerCamera;
	HalfHeightCache = InHalfHeight;
	RadiusCache = InRadius;

	SetComponentTickEnabled(true);
}

void UPlayerMovementComponent::TickComponent(float InDeltaTime, ELevelTick InTickType, FActorComponentTickFunction* InThisTickFunction)
{
	Super::TickComponent(InDeltaTime, InTickType, InThisTickFunction);

	check(UpdatedComponentCache.IsValid());

	// Gravity
	
	const FVector GravityDelta = FVector::DownVector * GravitySpeed * InDeltaTime;
	FHitResult HitResult;
	UpdatedComponentCache->AddWorldOffset(GravityDelta, true, &HitResult);
	
	bIsGrounded = HitResult.bBlockingHit;

	if (!bIsGrounded || MovementInput.IsZero())
	{
		Decelerate(InDeltaTime);
		return;
	}

	// Rotation
	
	RotateToMovement(InDeltaTime);

	// Movement

	Accelerate(InDeltaTime);
	const float MovementDeltaLength = RawSpeed * InDeltaTime;
	
	const FVector MovementDelta = FVector(MovementInput, 0.0f) * MovementDeltaLength;
	UpdatedComponentCache->AddWorldOffset(MovementDelta, true, &HitResult);

	if (!HitResult.bBlockingHit)
	{
		return;
	}

	// Slide

	const float EffectiveMovementDeltaLength = SlideAlongSurface(MovementDelta, HitResult.Normal, 1 - HitResult.Time);
	EffectiveSpeed = RawSpeed * EffectiveMovementDeltaLength / MovementDeltaLength;
}

void UPlayerMovementComponent::StartMouseMovement()
{
	// bBlockMouseMovement = GetWorld()->GetGameInstance()->GetSubsystem<UInteractionManager>()->HoveredInteractableExists();
}

void UPlayerMovementComponent::UpdateMouseMovement()
{
	check(UpdatedComponentCache.IsValid() && PlayerControllerCache.IsValid());

	if (bBlockMouseMovement)
	{
		RE_LOG("PlayerMovementComponent | Mouse movement blocked by HoveredInteractable.");
		MovementInput = FVector2D::ZeroVector;
		return;
	}

	FHitResult HitResult;
	PlayerControllerCache->GetHitResultUnderCursor(FTraceChannels::MovementPlane, false, HitResult);

	if (!HitResult.bBlockingHit)
	{
		RE_WARN("PlayerMovementComponent | GetHitResultUnderCursor() for mouse movement failed.");
		MovementInput = FVector2D::ZeroVector;
		return;
	}

	const FVector2D PlanarDelta = FVector2D(HitResult.ImpactPoint - UpdatedComponentCache->GetComponentLocation());
	const float PlanarDeltaSquaredLength = PlanarDelta.SquaredLength();
	
	MovementInput =
	(
		PlanarDeltaSquaredLength > FMath::Square(RadiusCache)
		? PlanarDelta * FMath::InvSqrt(PlanarDeltaSquaredLength)
		: FVector2D::ZeroVector
	);
}

void UPlayerMovementComponent::CompleteMovement()
{
	MovementInput = FVector2D::ZeroVector;
}

void UPlayerMovementComponent::UpdateGamepadMovement(const FInputActionValue& InValue)
{
	check(UpdatedComponentCache.IsValid() && PlayerControllerCache.IsValid() && PlayerCameraCache.IsValid());
	
	if (!InValue.IsNonZero())
	{
		MovementInput = FVector2D::ZeroVector;
		return;
	}
	
	const FVector2D AxisValue = InValue.Get<FVector2D>();
	
	const FRotator Rotator = FRotator
	(
		0.0f,
		PlayerCameraCache->GetActorRotation().Yaw,
		0.0f
	);
	
	const FVector WorldMovementVector =
	(
		FRotationMatrix(Rotator).GetUnitAxis(EAxis::X) * AxisValue.Y
		+
		FRotationMatrix(Rotator).GetUnitAxis(EAxis::Y) * AxisValue.X
	);
	
	MovementInput = FVector2D(WorldMovementVector).GetSafeNormal();
}

void UPlayerMovementComponent::Decelerate(float InDeltaTime)
{
	RawSpeed = FMath::Max
	(
		RawSpeed - Deceleration * InDeltaTime,
		0.0f
	);

	EffectiveSpeed =
	(
		EffectiveSpeed < RawSpeed
		?
		EffectiveSpeed
		:
		RawSpeed
	);
}

void UPlayerMovementComponent::RotateToMovement(float InDeltaTime)
{
	check(UpdatedComponentCache.IsValid());
	
	UpdatedComponentCache->SetWorldRotation
	(
		FMath::RInterpTo
		(
			UpdatedComponentCache->GetComponentRotation(),
			FVector(MovementInput, 0.0f).Rotation(),
			InDeltaTime,
			TurnRate
		)
	);
}

void UPlayerMovementComponent::Accelerate(float InDeltaTime)
{
	EffectiveSpeed = RawSpeed = FMath::Min
	(
		RawSpeed + Acceleration * InDeltaTime,
		SpeedLimit
	);
}

float UPlayerMovementComponent::SlideAlongSurface(const FVector& InDelta, const FVector& InSurfaceNormal, float InTime, int32 InMaxSurfaceCount)
{
	check(UpdatedComponentCache.IsValid());
	
	float EffectiveDeltaLength = 0.0f;
	FVector FirstSurfaceNormal = InSurfaceNormal;

	if (!IsWalkableSurface(FirstSurfaceNormal))
	{
		FirstSurfaceNormal = FirstSurfaceNormal.GetSafeNormal2D();
	}

	const FVector FirstSlideDelta = FVector::VectorPlaneProject(InDelta, FirstSurfaceNormal) * InTime;

	if
	(
		FirstSlideDelta.IsZero()
		||
		(FirstSlideDelta | InDelta) <= 0.0f
	)
	{
		return EffectiveDeltaLength;
	}
	
	FVector CurrentSurfaceNormal = FirstSurfaceNormal;
	FVector CurrentSlideDelta = FirstSlideDelta;

	for (int32 i = 0; i < InMaxSurfaceCount; ++i)
	{
		const float CurrentSlideDeltaLength = CurrentSlideDelta.Length();

		// Block small slides to avoid spacewalking.
		
		if (CurrentSlideDeltaLength < 0.5f)
		{
			return EffectiveDeltaLength;
		}
		
		FHitResult HitResult;
		UpdatedComponentCache->AddWorldOffset(CurrentSlideDelta, true, &HitResult);
		
		EffectiveDeltaLength += CurrentSlideDeltaLength * HitResult.Time;
		
		if (!HitResult.bBlockingHit)
		{
			break;
		}

		FVector NextSurfaceNormal = HitResult.Normal;

		if (!IsWalkableSurface(NextSurfaceNormal))
		{
			NextSurfaceNormal = NextSurfaceNormal.GetSafeNormal2D();
		}

		const FVector NextSlideDelta = TwoWallAdjust
		(
			CurrentSurfaceNormal,
			NextSurfaceNormal,
			CurrentSlideDelta,
			HitResult.Time
		);

		if
		(
			NextSlideDelta.IsZero()
			||
			(NextSlideDelta | InDelta) <= 0.0f
		)
		{
			break;
		}

		CurrentSurfaceNormal = NextSurfaceNormal;
		CurrentSlideDelta = NextSlideDelta;
	}

	return EffectiveDeltaLength;
}

bool UPlayerMovementComponent::IsWalkableSurface(const FVector& InSurfaceNormal)
{
	return InSurfaceNormal.Z >= 0.5f;
}

FVector UPlayerMovementComponent::TwoWallAdjust(const FVector& InCurrentSurfaceNormal, const FVector& InNextSurfaceNormal, const FVector& InCurrentSlideDelta, float InTime)
{
	FVector NextSlideDelta = FVector::ZeroVector;

	if ((InCurrentSurfaceNormal | InNextSurfaceNormal) <= 0.0f)
	{
		NextSlideDelta = (InNextSurfaceNormal ^ InCurrentSurfaceNormal).GetSafeNormal();
		NextSlideDelta = (InCurrentSlideDelta | NextSlideDelta) * NextSlideDelta * (1.0f - InTime);

		if ((InCurrentSlideDelta | NextSlideDelta) < 0.0f)
		{
			NextSlideDelta = -NextSlideDelta;
		}
	}
	
	else
	{
		NextSlideDelta = FVector::VectorPlaneProject(InCurrentSlideDelta, InNextSurfaceNormal) * (1.0f - InTime);
		
		if ((InCurrentSlideDelta | NextSlideDelta) <= 0.0f)
		{
			NextSlideDelta = FVector::ZeroVector;
		}
		
		else if (FMath::Abs((InCurrentSurfaceNormal | InNextSurfaceNormal) - 1.0f) < UE_KINDA_SMALL_NUMBER)
		{
			NextSlideDelta += InNextSurfaceNormal * 0.01f;
		}
	}
	
	return NextSlideDelta;
}