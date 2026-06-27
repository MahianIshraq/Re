#include "Player/PlayerMovementComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "InputAction.h"
#include "Struct/Asset/FInputAssetPaths.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "Engine/HitResult.h"
#include "Components/SceneComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Game/Manager/PointerManager.h"
#include "Engine/World.h"
#include "Engine/GameInstance.h"
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
	
	InEnhancedInputComponent->BindAction(IA_MouseMove, ETriggerEvent::Triggered, this, &UPlayerMovementComponent::UpdateMouseMovement);
	InEnhancedInputComponent->BindAction(IA_GamepadMove, ETriggerEvent::Triggered, this, &UPlayerMovementComponent::UpdateGamepadMovement);

	RE_LOG("PlayerMovementComponent | Input setup complete.");
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
	
	RE_LOG("PlayerMovementComponent | Setup complete.");
}

void UPlayerMovementComponent::TickComponent(float InDeltaTime, ELevelTick InTickType, FActorComponentTickFunction* InThisTickFunction)
{
	Super::TickComponent(InDeltaTime, InTickType, InThisTickFunction);
	check(UpdatedComponentCache.IsValid());
	FHitResult HitResult;

	// Gravity
	
	const FVector GravityDelta = FVector::DownVector * GravitySpeed * InDeltaTime;
	UpdatedComponentCache->AddWorldOffset(GravityDelta, true, &HitResult);
	
	bIsGrounded = HitResult.bBlockingHit;

	if (!bIsGrounded)
	{
		Decelerate(InDeltaTime);
		return;
	}

	if (!IsWalkableSurface(HitResult.Normal))
	{
		const FVector GravitySlideDelta = FVector::VectorPlaneProject(GravityDelta, HitResult.Normal) * (1 - HitResult.Time);
		UpdatedComponentCache->AddWorldOffset(GravitySlideDelta * GravitySlideFactor, true);
	}
	
	// Consume pending movement.

	if (!bIsMovementPending)
	{
		Decelerate(InDeltaTime);
		return;
	}

	bIsMovementPending = false;

	//	Ignore zero MovementInput.

	if (MovementInput.IsZero())
	{
		Decelerate(InDeltaTime);
		return;
	}

	// Store old location for EffectiveSpeed adjustment if there's sliding.

	const FVector OldLocation = UpdatedComponentCache->GetComponentLocation();

	// Rotation

	RotateToMovement(InDeltaTime);

	// Movement
	
	RawSpeed = FMath::Min
	(
		RawSpeed + Acceleration * InDeltaTime,
		MaxSpeed
	);

	const FVector MovementDirection = FVector(MovementInput, 0.0f);
	const float MovementDeltaLength = RawSpeed * InDeltaTime;

	const FVector MovementDelta = MovementDirection * MovementDeltaLength;
	UpdatedComponentCache->AddWorldOffset(MovementDelta, true, &HitResult);

	if (!HitResult.bBlockingHit)
	{
		EffectiveSpeed = RawSpeed;
		return;
	}

	// Physics

	if
	(
		UPrimitiveComponent* HitComponent = HitResult.Component.Get();
		HitComponent && HitComponent->IsSimulatingPhysics()
	)
	{
		HitComponent->AddImpulseAtLocation
		(
			MovementDirection * RawSpeed * ImpulseFactor,
			HitResult.ImpactPoint
		);

		EffectiveSpeed = RawSpeed;
		return;
	}

	// Slide

	SlideAlongSurface(MovementDelta, HitResult.Normal, 1 - HitResult.Time);

	const float TargetEffectiveSpeed =
	(
		RawSpeed
		*
		(UpdatedComponentCache->GetComponentLocation() - OldLocation).Size()
		/
		FMath::Max(MovementDeltaLength, 0.001f)
	);
	
	if (TargetEffectiveSpeed > EffectiveSpeed)
	{
		EffectiveSpeed = FMath::Min
		(
			EffectiveSpeed + Acceleration * InDeltaTime,
			TargetEffectiveSpeed
		);
	}
	
	else
	{
		EffectiveSpeed = FMath::Max
		(
			EffectiveSpeed - Deceleration * InDeltaTime,
			TargetEffectiveSpeed
		);
	}
}

void UPlayerMovementComponent::UpdateMouseMovement()
{
	check(PlayerControllerCache.IsValid() && UpdatedComponentCache.IsValid());

	if (GetWorld()->GetGameInstance()->GetSubsystem<UPointerManager>()->IsCaptured())
	{
		RE_LOG("PlayerMovementComponent | Mouse movement blocked by pointer capture.");
		MovementInput = FVector2D::ZeroVector;
		bIsMovementPending = false;
		return;
	}

	FHitResult HitResult;
	PlayerControllerCache->GetHitResultUnderCursor(FTraceChannels::MovementPlane, false, HitResult);

	if (!HitResult.bBlockingHit)
	{
		RE_WARN("PlayerMovementComponent | GetHitResultUnderCursor() for mouse movement failed.");
		MovementInput = FVector2D::ZeroVector;
		bIsMovementPending = false;
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

	bIsMovementPending = true;
}

void UPlayerMovementComponent::UpdateGamepadMovement(const FInputActionValue& InValue)
{
	check(PlayerCameraCache.IsValid());
	
	if (!InValue.IsNonZero())
	{
		MovementInput = FVector2D::ZeroVector;
		bIsMovementPending = false;
		return;
	}
	
	const FVector2D AxisVector = InValue.Get<FVector2D>();
	
	const FRotator Rotator = FRotator
	(
		0.0f,
		PlayerCameraCache->GetActorRotation().Yaw,
		0.0f
	);
	
	const FVector WorldMovementVector =
	(
		FRotationMatrix(Rotator).GetUnitAxis(EAxis::X) * AxisVector.Y
		+
		FRotationMatrix(Rotator).GetUnitAxis(EAxis::Y) * AxisVector.X
	);
	
	MovementInput = FVector2D(WorldMovementVector).GetSafeNormal();
	bIsMovementPending = true;
}

void UPlayerMovementComponent::Decelerate(float InDeltaTime)
{
	EffectiveSpeed = RawSpeed = FMath::Max
	(
		RawSpeed - Deceleration * InDeltaTime,
		0.0f
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

void UPlayerMovementComponent::SlideAlongSurface(const FVector& InDelta, const FVector& InSurfaceNormal, float InTime, int32 InMaxSlideCount)
{
	check(UpdatedComponentCache.IsValid());

	constexpr float MinimumSlide = 0.5f;
	FVector FirstSurfaceNormal = InSurfaceNormal;

	// Treat unwalkable surfaces as vertical.

	if (!IsWalkableSurface(FirstSurfaceNormal))
	{
		FirstSurfaceNormal = FirstSurfaceNormal.GetSafeNormal2D();
	}

	const FVector FirstSlideDelta = FVector::VectorPlaneProject(InDelta, FirstSurfaceNormal) * InTime;

	// Block small slides.

	if
	(
		FirstSlideDelta.Length() < MinimumSlide
		||
		(FirstSlideDelta | InDelta) <= 0.0f
	)
	{
		return;
	}
	
	FVector CurrentSurfaceNormal = FirstSurfaceNormal;
	FVector CurrentSlideDelta = FirstSlideDelta;
	FHitResult HitResult;
	
	for (int32 i = 0; i < InMaxSlideCount; ++i)
	{
		UpdatedComponentCache->AddWorldOffset(CurrentSlideDelta, true, &HitResult);
		
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
			1.0f - HitResult.Time
		);

		if
		(
			NextSlideDelta.Length() < MinimumSlide
			||
			(NextSlideDelta | InDelta) <= 0.0f
		)
		{
			break;
		}

		CurrentSurfaceNormal = NextSurfaceNormal;
		CurrentSlideDelta = NextSlideDelta;
	}
}

bool UPlayerMovementComponent::IsWalkableSurface(const FVector& InSurfaceNormal)
{
	return InSurfaceNormal.Z >= 0.5f; // FMath::Cos(60.0f)
}

FVector UPlayerMovementComponent::TwoWallAdjust(const FVector& InCurrentSurfaceNormal, const FVector& InNextSurfaceNormal, const FVector& InCurrentSlideDelta, float InTime)
{
	FVector NextSlideDelta = FVector::ZeroVector;

	if ((InCurrentSurfaceNormal | InNextSurfaceNormal) <= 0.0f)
	{
		NextSlideDelta = (InNextSurfaceNormal ^ InCurrentSurfaceNormal).GetSafeNormal();
		NextSlideDelta = (InCurrentSlideDelta | NextSlideDelta) * NextSlideDelta * InTime;

		if ((InCurrentSlideDelta | NextSlideDelta) < 0.0f)
		{
			NextSlideDelta = -NextSlideDelta;
		}
	}
	
	else
	{
		NextSlideDelta = FVector::VectorPlaneProject(InCurrentSlideDelta, InNextSurfaceNormal) * InTime;
		
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