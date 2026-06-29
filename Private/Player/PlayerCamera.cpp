#include "Player/PlayerCamera.h"
#include "Components/SceneComponent.h"
#include "Camera/CameraComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "InputAction.h"
#include "Struct/Asset/FInputAssetPaths.h"
#include "Materials/MaterialInterface.h"
#include "Struct/Asset/FMaterialPaths.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/PlayerController.h"
#include "Game/Manager/SaveManager.h"
#include "Game/Manager/InputManager.h"
#include "Engine/GameInstance.h"
#include "Game/ReSaveGame.h"
#include "Game/Manager/PointerManager.h"
#include "Game/ReLog.h"

APlayerCamera::APlayerCamera()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
	
	// PivotComponent
	
	PivotComponent = CreateDefaultSubobject<USceneComponent>(TEXT("PivotComponent"));
	RootComponent = PivotComponent;

	// CameraComponent

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(RootComponent);

	CameraComponent->SetProjectionMode(ECameraProjectionMode::Orthographic);
	CameraComponent->SetOrthoWidth(OrthoWidth);
	CameraComponent->SetAutoCalculateOrthoPlanes(true);
	CameraComponent->SetAutoPlaneShift(AutoPlaneShift);
	CameraComponent->SetUseCameraHeightAsViewTarget(true);
	
	CameraComponent->SetAspectRatio(AspectRatio);
	CameraComponent->SetConstraintAspectRatio(true);

	// IA | Overworld

	IA_MousePan = ConstructorHelpers::FObjectFinder<UInputAction>(FInputAssetPaths::IA_MousePan).Object;
	IA_GamepadPan = ConstructorHelpers::FObjectFinder<UInputAction>(FInputAssetPaths::IA_GamepadPan).Object;
	IA_CenterView = ConstructorHelpers::FObjectFinder<UInputAction>(FInputAssetPaths::IA_CenterView).Object;
	IA_LeftRotateView = ConstructorHelpers::FObjectFinder<UInputAction>(FInputAssetPaths::IA_LeftRotateView).Object;
	IA_RightRotateView = ConstructorHelpers::FObjectFinder<UInputAction>(FInputAssetPaths::IA_RightRotateView).Object;
	IA_ToggleOverheadView = ConstructorHelpers::FObjectFinder<UInputAction>(FInputAssetPaths::IA_ToggleOverheadView).Object;

	IA_GamepadPan->ValueType = EInputActionValueType::Axis2D;

	// Post Process

	FPostProcessSettings& PostProcessSettings = CameraComponent->PostProcessSettings;

	PostProcessSettings.AddBlendable(ConstructorHelpers::FObjectFinder<UMaterialInterface>(FMaterialPaths::M_Tonemapper_PP).Object, 1.0f);
	PostProcessSettings.AddBlendable(ConstructorHelpers::FObjectFinder<UMaterialInterface>(FMaterialPaths::M_DepthFog_PP).Object, 1.0f);
	PostProcessSettings.AddBlendable(ConstructorHelpers::FObjectFinder<UMaterialInterface>(FMaterialPaths::M_Pixelizer_PP).Object, 1.0f);
	PostProcessSettings.AddBlendable(ConstructorHelpers::FObjectFinder<UMaterialInterface>(FMaterialPaths::M_DitheredLighting_PP).Object, 1.0f);

	PostProcessSettings.bOverride_LumenFinalGatherScreenTraces = true;
	PostProcessSettings.LumenFinalGatherScreenTraces = false;
	PostProcessSettings.bOverride_LumenReflectionsScreenTraces = true;
	PostProcessSettings.LumenReflectionsScreenTraces = false;
}

void APlayerCamera::SetupInput(UEnhancedInputComponent* InEnhancedInputComponent)
{
	check(InEnhancedInputComponent);
	
	InEnhancedInputComponent->BindAction(IA_GamepadPan, ETriggerEvent::Started, this, &APlayerCamera::GamepadPanStarted);
	InEnhancedInputComponent->BindAction(IA_GamepadPan, ETriggerEvent::Triggered, this, &APlayerCamera::GamepadPan);
	InEnhancedInputComponent->BindAction(IA_GamepadPan, ETriggerEvent::Completed, this, &APlayerCamera::GamepadPanCompleted);
	InEnhancedInputComponent->BindAction(IA_MousePan, ETriggerEvent::Started, this, &APlayerCamera::MousePanStarted);
	InEnhancedInputComponent->BindAction(IA_MousePan, ETriggerEvent::Triggered, this, &APlayerCamera::MousePan);
	InEnhancedInputComponent->BindAction(IA_MousePan, ETriggerEvent::Completed, this, &APlayerCamera::MousePanCompleted);
	InEnhancedInputComponent->BindAction(IA_CenterView, ETriggerEvent::Triggered, this, &APlayerCamera::CenterView);
	InEnhancedInputComponent->BindAction(IA_LeftRotateView, ETriggerEvent::Started, this, &APlayerCamera::LeftRotateViewPressed);
	InEnhancedInputComponent->BindAction(IA_LeftRotateView, ETriggerEvent::Completed, this, &APlayerCamera::LeftRotateViewReleased);
	InEnhancedInputComponent->BindAction(IA_RightRotateView, ETriggerEvent::Started, this, &APlayerCamera::RightRotateViewPressed);
	InEnhancedInputComponent->BindAction(IA_RightRotateView, ETriggerEvent::Completed, this, &APlayerCamera::RightRotateViewReleased);
	InEnhancedInputComponent->BindAction(IA_ToggleOverheadView, ETriggerEvent::Started, this, &APlayerCamera::ToggleOverheadViewPressed);
	InEnhancedInputComponent->BindAction(IA_ToggleOverheadView, ETriggerEvent::Completed, this, &APlayerCamera::ToggleOverheadViewReleased);
	
	RE_LOG("PlayerCamera | Input setup complete.");
}

void APlayerCamera::Setup(const AActor* InTargetActor, const APlayerController* InPlayerController)
{
	TargetActorCache = InTargetActor;
	PlayerControllerCache = InPlayerController;

	check(TargetActorCache.IsValid() && PlayerControllerCache.IsValid());

	TargetLocation = InterpolatedLocation = TargetActorCache->GetActorLocation();
	SetActorLocation(TargetLocation);
	TargetRotation = InterpolatedRotation = FRotator(BasePitch, BaseYaw, 0.0f);
	SetActorRotation(TargetRotation);

	SetActorTickEnabled(true);
	RE_LOG("PlayerCamera | Setup complete.");
}

void APlayerCamera::BeginPlay()
{
	Super::BeginPlay();

	// Delegate Bindings

	USaveManager* SaveManager = GetGameInstance()->GetSubsystem<USaveManager>();
	SaveManager->OnGameSavedDelegate.AddUObject(this, &APlayerCamera::OnGameSavedListener);
	SaveManager->OnGameLoadedDelegate.AddUObject(this, &APlayerCamera::OnGameLoadedListener);

	GetGameInstance()->GetSubsystem<UInputManager>()->OnInputDeviceChangedDelegate.AddUObject(this, &APlayerCamera::OnInputDeviceChangedListener);

#if WITH_EDITOR
	
	SetFolderPath(FName(TEXT("[2] Player")));
	SetActorLabel(TEXT("Player Camera"));
	
#endif
}

void APlayerCamera::Tick(float InDeltaTime)
{
	Super::Tick(InDeltaTime);

	TickLocation(InDeltaTime);
	TickRotation(InDeltaTime);
}

void APlayerCamera::OnGameSavedListener(UReSaveGame* InSaveGame)
{
	InSaveGame->PlayerCameraLocation = TargetLocation;
	InSaveGame->PlayerCameraRotation = TargetRotation;
}

void APlayerCamera::OnGameLoadedListener(const UReSaveGame* InSaveGame)
{
	TargetLocation = InterpolatedLocation = InSaveGame->PlayerCameraLocation;
	TargetRotation = InterpolatedRotation = InSaveGame->PlayerCameraRotation;
}

void APlayerCamera::OnInputDeviceChangedListener(EHardwareDevicePrimaryType InputDeviceType)
{
	if (InputDeviceType == EHardwareDevicePrimaryType::KeyboardAndMouse)
	{
		bIsGamepadPanning = false; 
	}
	
	else if (InputDeviceType == EHardwareDevicePrimaryType::Gamepad)
	{
		bIsMousePanning = false;
	}
}

void APlayerCamera::MousePanStarted()
{
	bIsMousePanning = true;

	UPointerManager* PointerManager = GetGameInstance()->GetSubsystem<UPointerManager>();
	PointerManager->SetPointer(EPointer::Grab);
	PointerManager->CapturePointer();
	
	PlayerControllerCache->GetMousePosition(LastMousePanPosition.X, LastMousePanPosition.Y);
}

void APlayerCamera::MousePan()
{
	if (!bIsMousePanning)
	{
		return;
	}
	
	FVector2D CurrentMousePanPosition = FVector2D::ZeroVector;
	
	if (!PlayerControllerCache->GetMousePosition(CurrentMousePanPosition.X, CurrentMousePanPosition.Y))
	{
		RE_WARN("PlayerCamera | Mouse pan | Illegal mouse position.");
		return;
	}

	ApplyPan(CurrentMousePanPosition - LastMousePanPosition);
	LastMousePanPosition = CurrentMousePanPosition;
}

void APlayerCamera::MousePanCompleted()
{
	bIsMousePanning = false;

	UPointerManager* PointerManager = GetGameInstance()->GetSubsystem<UPointerManager>();
	PointerManager->ReleasePointer();
	PointerManager->SetPointer(EPointer::Default);
}

void APlayerCamera::GamepadPanStarted()
{
	bIsGamepadPanning = true;
}

void APlayerCamera::GamepadPan(const FInputActionValue& InValue)
{
	if (!bIsGamepadPanning)
	{
		return;
	}
	
	if (InValue.IsNonZero(0.5f))
	{
		ApplyPan(InValue.Get<FVector2D>() * GamepadPanFactor);
	}
}

void APlayerCamera::GamepadPanCompleted()
{
	bIsGamepadPanning = false;
}

void APlayerCamera::CenterView()
{
	if (!bIsMousePanning && !bIsGamepadPanning)
	{
		TargetLocation = TargetActorCache->GetActorLocation();
	}
}

void APlayerCamera::LeftRotateViewPressed()
{
	GetGameInstance()->GetSubsystem<UInputManager>()->SetActiveClickableInputAction(IA_LeftRotateView);
}

void APlayerCamera::LeftRotateViewReleased()
{
	if (!GetGameInstance()->GetSubsystem<UInputManager>()->TryConsumeActiveClickableInputAction(IA_LeftRotateView))
	{
		return;
	}
	
	if (bInvertRotation)
	{
		TargetRotation.Yaw -= RotationDelta;
	}
	else
	{
		TargetRotation.Yaw += RotationDelta;
	}
}

void APlayerCamera::RightRotateViewPressed()
{
	GetGameInstance()->GetSubsystem<UInputManager>()->SetActiveClickableInputAction(IA_RightRotateView);
}

void APlayerCamera::RightRotateViewReleased()
{
	if (!GetGameInstance()->GetSubsystem<UInputManager>()->TryConsumeActiveClickableInputAction(IA_RightRotateView))
	{
		return;
	}
	
	if (bInvertRotation)
	{
		TargetRotation.Yaw += RotationDelta;
	}
	else
	{
		TargetRotation.Yaw -= RotationDelta;
	}
}

void APlayerCamera::ToggleOverheadViewPressed()
{
	GetGameInstance()->GetSubsystem<UInputManager>()->SetActiveClickableInputAction(IA_ToggleOverheadView);
}

void APlayerCamera::ToggleOverheadViewReleased()
{
	if (!GetGameInstance()->GetSubsystem<UInputManager>()->TryConsumeActiveClickableInputAction(IA_ToggleOverheadView))
	{
		return;
	}

	bIsOverheadView = !bIsOverheadView;
	TargetRotation.Pitch = bIsOverheadView ? OverheadPitch : BasePitch;
}

void APlayerCamera::TickLocation(float InDeltaTime)
{
	// Auto-center
	
	if
	(
		FVector2D ScreenPosition = FVector2D::ZeroVector;
		PlayerControllerCache->ProjectWorldLocationToScreen(TargetActorCache->GetActorLocation(), ScreenPosition)
	)
	{
		int32 ViewportSizeX = 0, ViewportSizeY = 0;
		PlayerControllerCache->GetViewportSize(ViewportSizeX, ViewportSizeY);
		const float AutoCenterPadding = FMath::Min(ViewportSizeX, ViewportSizeY) * AutoCenterPaddingRatio;

		if
		(
			ScreenPosition.X < AutoCenterPadding ||
			ScreenPosition.X > ViewportSizeX - AutoCenterPadding ||
			ScreenPosition.Y < AutoCenterPadding ||
			ScreenPosition.Y > ViewportSizeY - AutoCenterPadding
		)
		{
			CenterView();
		}
	}

	// Execute
	
	if (bEnableLocationLag)
	{
		InterpolatedLocation = FMath::VInterpTo
		(
			InterpolatedLocation,
			TargetLocation,
			InDeltaTime,
			LocationLagSpeed
		);
	}
	else
	{
		InterpolatedLocation = TargetLocation;
	}

	SetActorLocation(InterpolatedLocation);
}

void APlayerCamera::TickRotation(float InDeltaTime)
{
	// Execute
	
	if (bEnableRotationLag)
	{
		InterpolatedRotation = FMath::RInterpTo
		(
			InterpolatedRotation,
			TargetRotation,
			InDeltaTime,
			RotationLagSpeed
		);
	}
	else
	{
		InterpolatedRotation = TargetRotation;
	}

	SetActorRotation(InterpolatedRotation);
}

void APlayerCamera::ApplyPan(const FVector2D& InPanDelta)
{
	if (InPanDelta.IsNearlyZero(0.5f))
	{
		return;
	}
	
	const FVector ForwardVector = GetActorForwardVector().GetSafeNormal2D();
	const FVector RightVector = GetActorRightVector().GetSafeNormal2D();
	
	int32 ViewportSizeX = 0, ViewportSizeY = 0;
	PlayerControllerCache->GetViewportSize(ViewportSizeX, ViewportSizeY);
	
	if (ViewportSizeX <= 0 || ViewportSizeY <= 0)
	{
		RE_WARN("PlayerCamera | Pan | Illegal viewport size.");
		return;
	}
	
	const float PitchFactor = FMath::Sin(FMath::DegreesToRadians(-InterpolatedRotation.Pitch));
	check(!FMath::IsNearlyZero(PitchFactor, 0.1f));
	
	const float PanSpeedX = OrthoWidth / static_cast<float>(ViewportSizeX);
	const float PanSpeedY = OrthoWidth / AspectRatio / PitchFactor / static_cast<float>(ViewportSizeY);

	FVector PannedLocation = InterpolatedLocation + (ForwardVector * InPanDelta.Y * PanSpeedY) - (RightVector * InPanDelta.X * PanSpeedX);
	const FVector TargetActorLocation = TargetActorCache->GetActorLocation();
	
	FVector WorldDelta = PannedLocation - TargetActorLocation;
	WorldDelta.Z = 0.0f;

	if (WorldDelta.SizeSquared() > FMath::Square(MaxPanDistance))
	{
		PannedLocation = TargetActorLocation + WorldDelta.GetSafeNormal() * MaxPanDistance;
	}

	TargetLocation = InterpolatedLocation = PannedLocation;
	SetActorLocation(TargetLocation);
}